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

#include "patheditor/pointhandle.hpp"

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include "patheditor/pathpoint.hpp"
#include "patheditor/pointcontextmenu.hpp"

using namespace patheditor;

PointHandle::PointHandle(PathPoint *point, int size, const QBrush &brush, QGraphicsItem *parent)
    : QGraphicsEllipseItem(0, 0, size, size, parent)
{
    _point = point;
    _originToCenter.setX(this->rect().width()/2);
    _originToCenter.setY(this->rect().height()/2);

    setCenter(_point);

    this->setBrush(brush);
    this->setFlag(QGraphicsItem::ItemIsMovable);
}

void PointHandle::setCenter(QPointF *point)
{
    this->setPos(*point - _originToCenter);
}

void PointHandle::setCenter(qreal &xpos, qreal &ypos)
{
    this->setPos(xpos - _originToCenter.rx(), ypos - _originToCenter.ry());
}

void PointHandle::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (_point->visible())
        QGraphicsEllipseItem::paint(painter, option, widget);
}

void PointHandle::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    _point->select();
    QGraphicsEllipseItem::mousePressEvent(event);
}

void PointHandle::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    qreal rx(this->pos().rx() + _originToCenter.rx());
    qreal ry(this->pos().ry() + _originToCenter.ry());
    _point->setRestrictedPos(rx, ry);
    _point->onPointDrag(event);
    QGraphicsEllipseItem::mouseMoveEvent(event);
}

void PointHandle::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    setCenter(_point);

    switch (event->button())
    {
    case Qt::RightButton:
        // TODO show dropdown (same action on double click?)
        {
            PointContextMenu menu(_point);
            menu.Show(event->buttonDownScreenPos(Qt::RightButton));
        }
        break;

    default:
        break;
    }

    _point->onPointRelease(event);
    QGraphicsEllipseItem::mouseReleaseEvent(event);
}
