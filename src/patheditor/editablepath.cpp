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

using namespace patheditor;

EditablePath::EditablePath(QSharedPointer<Path> path, QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    _editable = true;

    _path = path;
    foreach(QSharedPointer<PathItem> item, _path->pathItems())
        onAppend(item.data());
    connect(path.data(), SIGNAL(onAppend(patheditor::PathItem*)), this, SLOT(onAppend(patheditor::PathItem*)));

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
    if (isVisible() && !_path->pathItems().isEmpty())
    {
        QSharedPointer<QPainterPath> newPainterPath(new QPainterPath(*(_path->pathItems().first()->startPoint())));

        foreach(QSharedPointer<PathItem> item, _path->pathItems())
        {
            item->paintPathItem(&_settings, newPainterPath.data(), painter, option, widget, _editable);
        }

        painter->setPen(_settings.linePen());
        painter->drawPath(*(newPainterPath.data()));

        _painterPath.swap(newPainterPath);

        if (_firstPaint)
        {
            _firstPaint = false;
            emitPathReleased();
        }
    }
}

Path *EditablePath::path()
{
    return _path.data();
}

QSharedPointer<QPainterPath> EditablePath::painterPath()
{
    return _painterPath;
}

QPointF EditablePath::pointAtPercent(qreal t)
{
    return _path->pointAtPercent(t);
}

bool EditablePath::released()
{
    return _released;
}

void EditablePath::setEditable(bool editable)
{
    _editable = editable;

    foreach (QSharedPointer<PathItem> item, _path->pathItems())
    {
        item->startPoint()->handle()->setVisible(editable);
        item->endPoint()->handle()->setVisible(editable);

        foreach (QSharedPointer<ControlPoint> point, item->controlPoints())
        {
            point->handle()->setVisible(editable);
        }
    }
}

bool EditablePath::editable()
{
    return _editable;
}

EditablePath::~EditablePath()
{
}

void EditablePath::onAppend(PathItem *pathItem)
{
    // Add the startpoint pointHandle
    pathItem->startPoint()->createPointHandle(_settings, this);

    // Add the endpoint pointHandle
    pathItem->endPoint()->createPointHandle(_settings, this);

    // Add the controlPoint's pointHandles
    foreach(QSharedPointer<PathPoint> controlPoint, pathItem->controlPoints())
    {
        controlPoint->createPointHandle(_settings, this);
    }

    connectPoints(pathItem);
}

void EditablePath::onPointDrag(PathPoint* /*unused*/)
{
    _released = false;
    emitPathChanged();
    this->scene()->update();
}

void EditablePath::onPointRelease(PathPoint* /*unused*/)
{
    _released = true;
    emitPathReleased();
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

void EditablePath::emitPathChanged()
{
    _path->onPathChanged();
    emit pathChanged(this);
}

void EditablePath::emitPathReleased()
{
    _path->onPathReleased();
    emit pathReleased(this);
}
