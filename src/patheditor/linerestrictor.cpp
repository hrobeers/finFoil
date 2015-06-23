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

#include "linerestrictor.h"

using namespace patheditor;

LineRestrictor::LineRestrictor(const QPointF &point1, const QPointF &point2)
{
    _point1 = point1;
    _point2 = point2;
}

void LineRestrictor::restrictCoordinate(qreal *x, qreal *y)
{
    QPointF newPoint = closestPoint(QPointF(*x,*y), _point1, _point2);
    *x = newPoint.x();
    *y = newPoint.y();
}

QPointF LineRestrictor::closestPoint(const QPointF point, const QPointF &line_point1, const QPointF &line_point2)
{
    qreal r_numerator = (point.x()-line_point1.x())*(line_point2.x()-line_point1.x()) + (point.y()-line_point1.y())*(line_point2.y()-line_point1.y());
    qreal r_denomenator = (line_point2.x()-line_point1.x())*(line_point2.x()-line_point1.x()) + (line_point2.y()-line_point1.y())*(line_point2.y()-line_point1.y());
    qreal r = r_numerator / r_denomenator;

    return QPointF(line_point1.x() + r*(line_point2.x()-line_point1.x()),
                   line_point1.y() + r*(line_point2.y()-line_point1.y()));
}
