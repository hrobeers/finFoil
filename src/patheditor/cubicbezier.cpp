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

#include "cubicbezier.h"

#include <QPainter>
#include <QRectF>
#include "pathsettings.h"

using namespace patheditor;

CubicBezier::CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint)
    : PathItem(startPoint, endPoint)
{
    QPointF startToEnd = *endPoint - *startPoint;
    QPointF increment = startToEnd / 3;
    _cPoint1 = std::shared_ptr<ControlPoint>(new ControlPoint(0,0));
    _cPoint2 = std::shared_ptr<ControlPoint>(new ControlPoint(0,0));
    *_cPoint1 += *startPoint + increment;
    *_cPoint2 += *_cPoint1 + increment;

    _controlPoints.append(_cPoint1);
    _controlPoints.append(_cPoint2);
}

CubicBezier::CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<ControlPoint> controlPoint1,
                         std::shared_ptr<ControlPoint> controlPoint2, std::shared_ptr<PathPoint> endPoint)
    : PathItem(startPoint, endPoint)
{
    _cPoint1 = controlPoint1;
    _cPoint2 = controlPoint2;

    _controlPoints.append(_cPoint1);
    _controlPoints.append(_cPoint2);
}

std::shared_ptr<ControlPoint> CubicBezier::controlPoint1()
{
    return _cPoint1;
}

std::shared_ptr<ControlPoint> CubicBezier::controlPoint2()
{
    return _cPoint2;
}

QList<std::shared_ptr<ControlPoint> > CubicBezier::controlPoints()
{
    return _controlPoints;
}

QPointF CubicBezier::pointAtPercent(qreal t)
{
    // X(t) = (1-t)^3 * X0 + 3*(1-t)^2 * t * X1 + 3*(1-t) * t^2 * X2 + t^3 * X3

    qreal oneMt = 1-t;
    qreal tSq = t*t;
    qreal tCu = tSq * t;

    qreal xAtPercent = (oneMt*oneMt*oneMt) * _startPoint->x() + 3 * (oneMt*oneMt) * t * _cPoint1->x() +
            3 * oneMt * tSq * _cPoint2->x() + tCu * _endPoint->x();
    qreal yAtPercent = (oneMt*oneMt*oneMt) * _startPoint->y() + 3 * (oneMt*oneMt) * t * _cPoint1->y() +
            3 * oneMt * tSq * _cPoint2->y() + tCu * _endPoint->y();

    return QPointF(xAtPercent, yAtPercent);
}

QRectF CubicBezier::controlPointRect() const
{
    qreal left = qMin(qMin(_startPoint->x(), _endPoint->x()), qMin(_cPoint1->x(), _cPoint2->x()));
    qreal right = qMax(qMax(_startPoint->x(), _endPoint->x()), qMax(_cPoint1->x(), _cPoint2->x()));

    qreal top = qMin(qMin(_startPoint->y(), _endPoint->y()), qMin(_cPoint1->y(), _cPoint2->y()));
    qreal bottom = qMax(qMax(_startPoint->y(), _endPoint->y()), qMax(_cPoint1->y(), _cPoint2->y()));

    QPointF topLeft(left, top);
    QPointF bottomRight(right, bottom);

    QRectF retVal(topLeft, bottomRight);

    return retVal;
}

void CubicBezier::paintPathItem(PathSettings *settings, QPainterPath *totalPainterPath, QPainter *painter,
                        const QStyleOptionGraphicsItem * /*unused*/, QWidget * /*unused*/, bool editable)
{
    if (editable)
        paintControlPoints(settings, painter);

    totalPainterPath->cubicTo(*_cPoint1, *_cPoint2, *_endPoint);
}
