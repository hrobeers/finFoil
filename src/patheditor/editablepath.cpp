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

EditablePath::EditablePath(QSharedPointer<Path> path, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    _path = path;
    foreach(QSharedPointer<PathItem> item, _path->pathItems())
        onAppend(item.data());
    connect(path.data(), SIGNAL(onAppend(PathItem*)), this, SLOT(onAppend(PathItem*)));

    _firstPaint = true;
    _released = true;
    _settings = PathSettings::Default();
}

QRectF EditablePath::boundingRect() const
{
    return _path->controlPointRect();
}

void EditablePath::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!_path->pathItems().isEmpty())
    {
        QSharedPointer<QPainterPath> newPainterPath(new QPainterPath(*(_path->pathItems().first()->startPoint())));

        foreach(QSharedPointer<PathItem> item, _path->pathItems())
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
    return _path->pointAtPercent(t);
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

EditablePath::~EditablePath()
{
}

void EditablePath::onAppend(PathItem *pathItem)
{
    // Add the startpoint pointHandle
    pathItem->startPoint()->createPointHandle(_settings, this, scene());

    // Add the endpoint pointHandle
    pathItem->endPoint()->createPointHandle(_settings, this, scene());

    // Add the controlPoint's pointHandles
    foreach(QSharedPointer<PathPoint> controlPoint, pathItem->controlPoints())
    {
        controlPoint->createPointHandle(_settings, this, scene());
    }

    connectPoints(pathItem);
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
