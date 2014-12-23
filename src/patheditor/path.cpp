/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
 All rights reserved.
 
 BSD 2-Clause License
 
 Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
 
   * Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
   
   * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
   
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
****************************************************************************/

#include "path.h"
#include "pathtemplates.hpp"

#include <QVarLengthArray>
#include <QJsonArray>
#include <QPainter>
#include "controlpoint.h"
#include "pathitem.h"
#include "line.h"
#include "cubicbezier.h"
#include "pathsettings.h"

using namespace patheditor;

Path::Path(QObject *parent) :
    QObject(parent)
{
}

void Path::append(std::shared_ptr<PathItem> pathItem)
{
    // Set pathItem next and prev
    if (!_pathItemList.isEmpty())
    {
        std::shared_ptr<PathItem> last = _pathItemList.last();
        last->setNextPathItem(pathItem);
        pathItem->setPrevPathItem(last);
    }

    if (!_pathItemList.isEmpty())
    {
        pathItem->setStartPoint(_pathItemList.last()->endPoint());
    }

    emit onAppend(pathItem.get());
    _pathItemList.append(pathItem);
}

QList<std::shared_ptr<PathItem> > Path::pathItems()
{
    return _pathItemList;
}

QList<const PathItem *> Path::constPathItems() const
{
    return jenson::toConstList(_pathItemList);
}

QRectF Path::controlPointRect() const
{
    if (_pathItemList.count() <= 0)
        return QRectF(0,0,0,0);

    QRectF retVal = _pathItemList.first()->controlPointRect();
    foreach(std::shared_ptr<PathItem> item, _pathItemList)
    {
        retVal |= item->controlPointRect();
    }

    return retVal;
}

QPointF Path::pointAtPercent(qreal t) const
{
    int pathItemCount = _pathItemList.count();
    qreal itemRange = 1/qreal(pathItemCount);

    int item = 0;
    while (t > itemRange && item < pathItemCount-1)
    {
        t -= itemRange;
        item++;
    }

    t = t/itemRange;

    if (t<0) t = 0;
    if (t>1) t = 1;

    return _pathItemList[item]->pointAtPercent(t);
}

qreal Path::minX(qreal *t_top) const
{
    return extreme<X, Min>(this, t_top);
}

qreal Path::maxX(qreal *t_top) const
{
    return extreme<X, Max>(this, t_top);
}

qreal Path::minY(qreal *t_top) const
{
    return extreme<Y, Min>(this, t_top);
}

qreal Path::maxY(qreal *t_top) const
{
    return extreme<Y, Max>(this, t_top);
}

qreal Path::area(int resolution) const
{
    qreal percStep = 1 / qreal(resolution);
    QVarLengthArray<QPointF, PATH_AREARES> points(resolution);
    qreal perc = 0;
    for (int i = 0; i < resolution; i++)
    {
        points[i] = pointAtPercent(perc);
        perc += percStep;
    }

    qreal area = 0;
    int j = 0;
    for (int i = 0; i < resolution; i++)
    {
        j = (i + 1) % resolution;
        area += points[i].x() * points[j].y();
        area -= points[j].x() * points[i].y();
    }

    return qAbs(area) / 2;
}

void Path::paint(QPainter *painter, bool editable, const PathSettings *settings)
{
    if (!pathItems().isEmpty())
    {
        if (!settings) settings = PathSettings::Default();

        QPainterPath painterPath(*(pathItems().first()->startPoint()));

        foreach(std::shared_ptr<PathItem> item, pathItems())
        {
            item->paintPathItem(&painterPath, painter, editable, settings);
        }

        painter->setPen(settings->linePen());
        painter->drawPath(painterPath);
    }
}

void Path::onPathChanged()
{
    emit pathChanged(this);
}

void Path::onPathReleased()
{
    emit pathReleased(this);
}


//
// PathSerializer
//

namespace patheditor {
    class PathSerializer : public jenson::JenSON::CustomSerializer<Path>
    {
    protected:
        virtual QJsonValue serializeImpl(const Path *object) const override;
        virtual sptr<Path> deserializeImpl(const QJsonValue *jsonValue, QString *errorMsg) const override;

    private:
        std::shared_ptr<PathItem> toPathItem(QJsonArray pathItemJson, std::shared_ptr<PathPoint> &previousEndPoint) const;

        void appendPntToJsonArray(const PathPoint *pnt, QJsonArray *array) const
        {
            array->append(pnt->x());
            array->append(pnt->y());
        }

        template <typename Tpnt>
        std::shared_ptr<Tpnt> takePoint(QJsonArray *pntArray) const
        {
            std::shared_ptr<Tpnt> retVal;

            QJsonValue xval = pntArray->takeAt(0), yval = pntArray->takeAt(0);

            if(xval.isDouble() && yval.isDouble())
            {
                retVal.reset(new Tpnt(xval.toDouble(), yval.toDouble()));
            }

            return retVal;
        }
    };
}

CUSTOMSERIALIZABLE(patheditor::Path, patheditor::PathSerializer, path)

QJsonValue PathSerializer::serializeImpl(const Path *object) const
{
    QJsonArray retVal;

    // Move to first point
    QJsonArray moveToStart;
    moveToStart.append(QStringLiteral("M"));
    appendPntToJsonArray(object->constPathItems().first()->constStartPoint(), &moveToStart);
    retVal.append(moveToStart);

    foreach (const PathItem *pathItem, object->constPathItems())
    {
        QJsonArray nestedArr;

        switch (pathItem->constControlPoints().count())
        {
        case 0:
            nestedArr.append(QStringLiteral("L"));
            break;

        case 2:
            nestedArr.append(QStringLiteral("C"));
            break;

        default:
            QString msg = QStringLiteral("PathSerializer has no implementation to serialize a path with ")
                    + pathItem->constControlPoints().count()
                    + QStringLiteral(" controlpoints.");
            throw jenson::SerializationException(msg);
        }

        foreach (const ControlPoint *pnt, pathItem->constControlPoints())
            appendPntToJsonArray(pnt, &nestedArr);

        appendPntToJsonArray(pathItem->constEndPoint(), &nestedArr);

        retVal.append(nestedArr);
    }

    return retVal;
}

std::shared_ptr<PathItem> PathSerializer::toPathItem(QJsonArray pathItemJson, std::shared_ptr<PathPoint> &previousEndPoint) const
{
    std::shared_ptr<PathItem> retVal;

    QString typeId = pathItemJson.takeAt(0).toString();

    if (typeId == QStringLiteral("M"))
    {
        previousEndPoint = takePoint<PathPoint>(&pathItemJson);
    }
    else if (typeId == QStringLiteral("L"))
    {
        auto endPnt = takePoint<PathPoint>(&pathItemJson);
        if (previousEndPoint && endPnt)
            retVal.reset(new Line(previousEndPoint, endPnt));
        previousEndPoint = endPnt;
    }
    else if (typeId == QStringLiteral("C"))
    {
        auto cPnt1 = takePoint<ControlPoint>(&pathItemJson);
        auto cPnt2 = takePoint<ControlPoint>(&pathItemJson);
        auto endPnt = takePoint<PathPoint>(&pathItemJson);
        if (previousEndPoint && cPnt1 && cPnt2 && endPnt)
            retVal.reset(new CubicBezier(previousEndPoint, cPnt1, cPnt2, endPnt));
        previousEndPoint = endPnt;
    }

    return retVal;
}

sptr<Path> PathSerializer::deserializeImpl(const QJsonValue *jsonValue, QString *errorMsg) const
{
    if (!jsonValue->isArray())
    {
        if (errorMsg) errorMsg->append("\n PathSerializer::deserializeImpl -> path is not a json array");
        return nullptr;
    }

    sptr<Path> path(new Path());

    QJsonArray array = jsonValue->toArray();

    std::shared_ptr<PathPoint> previousEndPoint(nullptr);
    foreach (QJsonValue value, array)
    {
        if (value.isArray())
        {
            std::shared_ptr<PathItem> pathItem = toPathItem(value.toArray(), previousEndPoint);

            if (pathItem)
                path->append(pathItem);
        }
    }

    return path;
}
