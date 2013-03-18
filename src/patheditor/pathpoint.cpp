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

#include <QGraphicsScene>
#include "pathsettings.h"

using namespace patheditor;

static QHash<QGraphicsScene*, PathPoint*> s_prevSelected;

PathPoint::PathPoint(qreal xpos, qreal ypos)
    : QPointF(xpos, ypos)
{
    _selected = false;
    _pointHandle = 0;
    _toFollowPoint = 0;
}

void PathPoint::setRestrictedPos(qreal &xpos, qreal &ypos)
{
    if (!_restrictor.isNull())
        _restrictor->restrictCoordinate(&xpos, &ypos);

    qreal dx = xpos - this->rx();
    qreal dy = ypos - this->ry();

    this->setX(xpos);
    this->setY(ypos);

    foreach(QWeakPointer<PathPoint> linkedPoint, _followingPoints)
    {
        if (!linkedPoint.isNull())
        {
            QSharedPointer<PathPoint> strongPnt = linkedPoint.toStrongRef();
            qreal rx(strongPnt->rx() + dx);
            qreal ry(strongPnt->ry() + dy);
            strongPnt->setPos(rx, ry);
        }
    }
}

void PathPoint::setRestrictor(QSharedPointer<Restrictor> restrictor)
{
    _restrictor = restrictor;
    setRestrictedPos(this->rx(), this->ry());
}

void PathPoint::createPointHandle(PathSettings &settings, QGraphicsItem *parent, QGraphicsScene *scene)
{
    replaceCurrentPointHandle(new PointHandle(this, settings.handleSize(), settings.pointBrush(), parent, scene));
}

void PathPoint::addFollowingPoint(QSharedPointer<PathPoint> point)
{
    point->_toFollowPoint = this;
    _followingPoints.append(point.toWeakRef());
}

bool PathPoint::visible()
{
    if (_toFollowPoint)
    {
        return _toFollowPoint->selected();
    }
    else
    {
        return true;
    }
}

void PathPoint::select()
{
    QGraphicsScene* scene = this->_pointHandle->scene();

    if (!_toFollowPoint)
        select(this, scene);
}

bool PathPoint::selected() const
{
    return _selected;
}

void PathPoint::replaceCurrentPointHandle(PointHandle *pointHandle)
{
    if (_pointHandle)
    {
        _pointHandle->scene()->removeItem(_pointHandle);
        delete _pointHandle;
    }

    _pointHandle = pointHandle;
}

void PathPoint::setPos(qreal &xpos, qreal &ypos)
{
    this->setX(xpos);
    this->setY(ypos);

    if (_pointHandle)
    {
        _pointHandle->setCenter(xpos, ypos);
    }
}

void PathPoint::select(PathPoint *point, QGraphicsScene *scene)
{
    if (s_prevSelected.contains(scene))
        s_prevSelected[scene]->_selected = false;

    s_prevSelected[scene] = point;
    point->_selected = true;
}
