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

#include "line.h"

#include <QPainter>
#include <QGraphicsItem>
#include <QRectF>
#include "pathpoint.h"
#include "pathsettings.h"

using namespace patheditor;

Line::Line(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint)
    : PathItem(startPoint, endPoint)
{
}

QList<std::shared_ptr<ControlPoint> > Line::controlPoints()
{
    return _controlPoints;
}

QPointF Line::pointAtPercent(qreal t)
{
    return (*_endPoint - *_startPoint) * t + *_startPoint;
}

QRectF Line::controlPointRect() const
{
    qreal left = qMin(_startPoint->x(), _endPoint->x());
    qreal right = qMax(_startPoint->x(), _endPoint->x());

    qreal top = qMin(_startPoint->y(), _endPoint->y());
    qreal bottom = qMax(_startPoint->y(), _endPoint->y());

    QPointF topLeft(left, top);
    QPointF bottomRight(right, bottom);

    QRectF retVal(topLeft, bottomRight);

    return retVal;
}

void Line::paintPathItem(PathSettings * /*unused*/, QPainterPath *totalPainterPath, QPainter * /*unused*/,
                 const QStyleOptionGraphicsItem * /*unused*/, QWidget * /*unused*/, bool /*unused*/)
{
    totalPainterPath->lineTo(*_endPoint);
}
