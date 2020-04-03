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

#include "patheditor/pathitem.hpp"

#include <QPainter>
#include "patheditor/pathsettings.hpp"
#include "patheditor/controlpoint.hpp"

using namespace patheditor;

std::shared_ptr<PathPoint> PathItem::startPoint()
{
    return _startPoint;
}

std::shared_ptr<PathPoint> PathItem::endPoint()
{
    return _endPoint;
}

void PathItem::setStartPoint(std::shared_ptr<PathPoint> startPoint)
{
    _startPoint = startPoint;

    if (controlPoints().count() >= 2 &&
        controlPoints().first()->toFollowPoint() != _startPoint.get())
    {
            _startPoint->addFollowingPoint(controlPoints().first());
    }
}

void PathItem::setEndPoint(std::shared_ptr<PathPoint> endPoint)
{
    _endPoint = endPoint;

    if (controlPoints().count() >= 2 &&
        controlPoints().last()->toFollowPoint() != _endPoint.get())
    {
        _endPoint->addFollowingPoint(controlPoints().last());
    }
}

const PathPoint *PathItem::constStartPoint() const
 {
    return _startPoint.get();
}

const PathPoint *PathItem::constEndPoint() const
 {
    return _endPoint.get();
 }

std::weak_ptr<PathItem> PathItem::nextPathItem() const
{
    return _nextPathItem;
}

std::weak_ptr<PathItem> PathItem::prevPathItem() const
{
    return _prevPathItem;
}

void PathItem::setNextPathItem(std::shared_ptr<PathItem> nextPathItem)
{
    _nextPathItem = nextPathItem;
}

void PathItem::setPrevPathItem(std::shared_ptr<PathItem> prevPathItem)
{
    _prevPathItem = prevPathItem;
}

void PathItem::paintControlPoints(QPainter *painter, const PathSettings *settings) const
{
    if (!settings) settings = PathSettings::Default();

    painter->setPen(settings->controlLinePen());
    QPainterPath painterPath;

    if (constStartPoint()->selected())
    {
        painterPath.moveTo(*constStartPoint());
        painterPath.lineTo(*(constControlPoints().first()));
    }

    if (constEndPoint()->selected())
    {
        painterPath.moveTo(*constEndPoint());
        painterPath.lineTo(*(constControlPoints().last()));
    }

    painter->drawPath(painterPath);
}

void PathItem::paintPathItem(QPainterPath *totalPainterPath, QPainter *painter, bool editable, const PathSettings *settings) const
{
    if (!settings) settings = PathSettings::Default();
    paintPathItemImpl(totalPainterPath, painter, editable, settings);
}

QList<QPointF> PathItem::points() const
{
  QList<QPointF> pnts;
  pnts.append(QPointF(constStartPoint()->x(), constStartPoint()->y()));
  for (auto p : constControlPoints())
    pnts.append(QPointF(p->x(), p->y()));
  pnts.append(QPointF(constEndPoint()->x(), constEndPoint()->y()));
  return pnts;
}
