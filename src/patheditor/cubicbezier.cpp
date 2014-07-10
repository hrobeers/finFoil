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
#include <boost/math/special_functions/pow.hpp>
#include "pathsettings.h"

using namespace patheditor;
using namespace boost::math;

CubicBezier::CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint)
{
    QPointF startToEnd = *endPoint - *startPoint;
    QPointF increment = startToEnd / 3;
    _cPoint1.reset(new ControlPoint(0,0));
    _cPoint2.reset(new ControlPoint(0,0));
    *_cPoint1 += *startPoint + increment;
    *_cPoint2 += *_cPoint1 + increment;

    _controlPoints.append(_cPoint1);
    _controlPoints.append(_cPoint2);

    setStartPoint(startPoint);
    setEndPoint(endPoint);
}

CubicBezier::CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<ControlPoint> controlPoint1,
                         std::shared_ptr<ControlPoint> controlPoint2, std::shared_ptr<PathPoint> endPoint)
{
    _cPoint1 = controlPoint1;
    _cPoint2 = controlPoint2;

    _controlPoints.append(_cPoint1);
    _controlPoints.append(_cPoint2);

    setStartPoint(startPoint);
    setEndPoint(endPoint);
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

const QList<const ControlPoint *> CubicBezier::constControlPoints() const
{
    return jenson::toConstList(_controlPoints);
}

QPointF CubicBezier::pointAtPercent(qreal t)
{
    // X(t) = (1-t)^3 * X0 + 3*(1-t)^2 * t * X1 + 3*(1-t) * t^2 * X2 + t^3 * X3

    qreal xAtPercent = pow<3>(1-t) * _startPoint->x() + 3 * pow<2>(1-t) * t * _cPoint1->x() +
            3 * (1-t) * pow<2>(t) * _cPoint2->x() + pow<3>(t) * _endPoint->x();

    qreal yAtPercent = pow<3>(1-t) * _startPoint->y() + 3 * pow<2>(1-t) * t * _cPoint1->y() +
            3 * (1-t) * pow<2>(t) * _cPoint2->y() + pow<3>(t) * _endPoint->y();

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
