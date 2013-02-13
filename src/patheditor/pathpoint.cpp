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

#include "pathpoint.h"
#include "pathsettings.h"
#include <QGraphicsScene>

using namespace patheditor;

PathPoint::PathPoint(qreal xpos, qreal ypos, PointType::e type)
    : QPointF(xpos, ypos)
{
    _pointHandle = NULL;

    _type = type;
}

void PathPoint::setRestrictedPos(qreal xpos, qreal ypos)
{
    if (!_restrictor.isNull())
        _restrictor->restrictCoordinate(&xpos, &ypos);

    qreal dx = xpos - this->rx();
    qreal dy = ypos - this->ry();

    this->setX(xpos);
    this->setY(ypos);

    foreach(QWeakPointer<PathPoint> linkedPoint, _linkedPoints)
    {
        if (!linkedPoint.isNull())
        {
            QSharedPointer<PathPoint> strongPnt = linkedPoint.toStrongRef();
            strongPnt->setPos(strongPnt->rx() + dx, strongPnt->ry() + dy);
        }
    }
}

void PathPoint::setRestrictor(QSharedPointer<Restrictor> &restrictor)
{
    _restrictor = restrictor;
}

void PathPoint::createPointHandle(PathSettings &settings, QGraphicsItem *parent, QGraphicsScene *scene)
{
    if (_pointHandle != NULL)
    {
        _pointHandle->scene()->removeItem(_pointHandle);
        delete _pointHandle;
    }

    switch (_type)
    {
    case PointType::Point:
        _pointHandle = new PointHandle(this, settings.pointBrush(), parent, scene);
        break;

    case PointType::ControlPoint:
        _pointHandle = new PointHandle(this, settings.controlPointBrush(), parent, scene);
        break;
    }
}

void PathPoint::addLinkedPoint(QWeakPointer<PathPoint> linkedPoint)
{
    _linkedPoints.append(linkedPoint);
}

void PathPoint::setPos(qreal xpos, qreal ypos)
{
    this->setX(xpos);
    this->setY(ypos);

    if (_pointHandle != NULL)
    {
        // taking adress of temporary is ok here, QPointF can be destroyed after setCenter
        _pointHandle->setCenter(&QPointF(xpos, ypos));
    }
}
