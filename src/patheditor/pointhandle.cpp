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

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "pointhandle.h"

using namespace patheditor;

PointHandle::PointHandle(QSharedPointer<RestrictablePoint> point,
                         QBrush &brush, QGraphicsItem *parent, QGraphicsScene *scene)
    : QGraphicsEllipseItem(0, 0, 10, 10, parent, scene)
{
    _point = point;
    _originToCenter.setX(this->rect().width()/2);
    _originToCenter.setY(this->rect().height()/2);

    setCenter(_point);

    this->setZValue(1);
    this->setBrush(brush);
    this->setFlag(QGraphicsItem::ItemIsMovable);
}

void PointHandle::setCenter(QSharedPointer<QPointF> point)
{
    this->setPos(*point - _originToCenter);
}

void PointHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    _point->setX(this->pos().rx() + _originToCenter.rx());
//    _point->setY(this->pos().ry() + _originToCenter.ry());
    _point->setRestrictedPos(this->pos().rx() + _originToCenter.rx(),
                             this->pos().ry() + _originToCenter.ry());
    this->scene()->update();
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void PointHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCenter(_point);
    this->scene()->update();
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}
