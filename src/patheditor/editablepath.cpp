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

#include "editablepath.h"

#include <QPainter>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include "hrlib/math/brent.hpp"
#include "pathfunctors.h"

using namespace patheditor;

EditablePath::EditablePath(QGraphicsItem * parent)
    : QGraphicsObject(parent)
{
    _firstPaint = true;
    _released = true;
    _settings = PathSettings::Default();
}

void EditablePath::append(QSharedPointer<PathItem> pathItem)
{
    // Set pathItem next and prev
    if (!_pathItemList.isEmpty())
    {
        QSharedPointer<PathItem> last = _pathItemList.last();
        last->setNextPathItem(pathItem);
        pathItem->setPrevPathItem(last);
    }

    // Add the controlPoint's pointHandles
    foreach(QSharedPointer<PathPoint> controlPoint, pathItem->controlPoints())
    {
        controlPoint->createPointHandle(_settings, this, scene());
    }

    if (!_pathItemList.isEmpty())
    {
        pathItem->setStartPoint(_pathItemList.last()->endPoint());
    }
    else
    {
        // Add the startpoint pointHandle
        pathItem->startPoint()->createPointHandle(_settings, this, scene());
    }

    // Add the endpoint pointHandle
    pathItem->endPoint()->createPointHandle(_settings, this, scene());

    // Link endpoints to controlpoints
    if (pathItem->controlPoints().count() >= 2)
    {
        pathItem->startPoint()->addFollowingPoint(pathItem->controlPoints().first());
        pathItem->endPoint()->addFollowingPoint(pathItem->controlPoints().last());
    }

    connectPoints(pathItem.data());
    _pathItemList.append(pathItem);
}

QRectF EditablePath::boundingRect() const
{
    if (_pathItemList.count() <= 0)
        return QRectF(0,0,0,0);

    QRectF retVal = _pathItemList.first()->boundingRect();
    foreach(QSharedPointer<PathItem> item, _pathItemList)
    {
        retVal |= item->boundingRect();
    }

    return retVal;
}

void EditablePath::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!_pathItemList.isEmpty())
    {
        QSharedPointer<QPainterPath> newPainterPath(new QPainterPath(*(_pathItemList.first()->startPoint())));

        foreach(QSharedPointer<PathItem> item, _pathItemList)
        {
            item->paintPathItem(&_settings, newPainterPath.data(), painter, option, widget);
        }

        painter->setPen(_settings.linePen());
        painter->drawPath(*(newPainterPath.data()));

        _painterPath.swap(newPainterPath);

        if (_firstPaint)
        {
            _firstPaint = false;
            emit pathChanged(this);
        }
    }
}

QSharedPointer<QPainterPath> EditablePath::painterPath()
{
    return _painterPath;
}

QPointF EditablePath::pointAtPercent(qreal t)
{
    int pathItemCount = _pathItemList.count();
    qreal itemRange = 1/qreal(pathItemCount);

    int item = 0;
    while (t > itemRange)
    {
        t -= itemRange;
        item++;
    }

    t = t/itemRange;

    return _pathItemList[item]->pointAtPercent(t);
}

qreal EditablePath::minY(qreal *t_top, qreal percTol)
{
    // pathfunctor
    f_yValueAtPercentEPath yOutline(this);

    // find the min of the path
    if (t_top == 0)
    {
        qreal t = 0.5;
        return hrlib::Brent::local_min(0, 1, percTol, yOutline, t);
    }
    else
        return hrlib::Brent::local_min(0, 1, percTol, yOutline, *t_top);
}

qreal EditablePath::area(int resolution)
{
    qreal percStep = 1 / qreal(resolution);
    QPointF points[resolution];
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

bool EditablePath::released()
{
    return _released;
}

void EditablePath::onPointDrag(PathPoint* /*unused*/)
{
    _released = false;
    emit pathChanged(this);
    this->scene()->update();
}

void EditablePath::onPointRelease(PathPoint* /*unused*/)
{
    _released = true;
    emit pathChanged(this);
    this->scene()->update();
}

void EditablePath::connectPoints(PathItem *pathItem)
{
    connect(pathItem->startPoint().data(), SIGNAL(pointDrag(PathPoint*)),
            this, SLOT(onPointDrag(PathPoint*)), Qt::UniqueConnection);
    connect(pathItem->startPoint().data(), SIGNAL(pointRelease(PathPoint*)),
            this, SLOT(onPointRelease(PathPoint*)), Qt::UniqueConnection);

    connect(pathItem->endPoint().data(), SIGNAL(pointDrag(PathPoint*)),
            this, SLOT(onPointDrag(PathPoint*)), Qt::UniqueConnection);
    connect(pathItem->endPoint().data(), SIGNAL(pointRelease(PathPoint*)),
            this, SLOT(onPointRelease(PathPoint*)), Qt::UniqueConnection);

    foreach(QSharedPointer<ControlPoint> cPoint, pathItem->controlPoints())
    {
        connect(cPoint.data(), SIGNAL(pointDrag(PathPoint*)),
                this, SLOT(onPointDrag(PathPoint*)), Qt::UniqueConnection);
        connect(cPoint.data(), SIGNAL(pointRelease(PathPoint*)),
                this, SLOT(onPointRelease(PathPoint*)), Qt::UniqueConnection);
    }
}
