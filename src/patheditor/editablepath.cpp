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

#include <QPainter>
#include "editablepath.h"

using namespace patheditor;

EditablePath::EditablePath(QGraphicsItem * parent, QGraphicsScene * scene)
    : QGraphicsItem(parent, scene)
{
}

void EditablePath::append(QSharedPointer<PathItem> pathItem)
{
    // reparent
    pathItem->setParentItem(this);

    if (_pathItemList.count() > 0)
    {
        pathItem->setStartPoint(_pathItemList.last()->endPoint());
    }
    else
    {
        // Add the startpoint pointHandle
        _pointHandleList.append(QSharedPointer<PointHandle>(
                                    new PointHandle(pathItem->startPoint(), this, scene())));
    }

    // Add the endpoint pointHandle
    _pointHandleList.append(QSharedPointer<PointHandle>(
                                new PointHandle(pathItem->endPoint(), this, scene())));

    // Add the controlPoint's pointHandles
    foreach(QSharedPointer<QPointF> controlPoint, pathItem->controlPoints())
    {
        _pointHandleList.append(QSharedPointer<PointHandle>(
                                    new PointHandle(controlPoint, this, scene())));
    }

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
    foreach(QSharedPointer<PathItem> item, _pathItemList)
    {
        item->paint(painter, option, widget);
    }
}
