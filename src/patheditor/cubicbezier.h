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

#ifndef CUBICBEZIER_H
#define CUBICBEZIER_H

#include "patheditorfwd/patheditorfwd.h"

#include "pathitem.h"

namespace patheditor
{
    /**
     * @brief The CubicBezier PathItem
     */
    class CubicBezier : public PathItem
    {
    public:
        explicit CubicBezier(qshared_ptr<PathPoint> startPoint, qshared_ptr<PathPoint> endPoint);

        explicit CubicBezier(qshared_ptr<PathPoint> startPoint, qshared_ptr<ControlPoint> controlPoint1,
                             qshared_ptr<ControlPoint> controlPoint2, qshared_ptr<PathPoint> endPoint);

        qshared_ptr<ControlPoint> controlPoint1();
        qshared_ptr<ControlPoint> controlPoint2();

        // implementing PathItem
        virtual QList<qshared_ptr<ControlPoint> > controlPoints() override;
        virtual const QList<const ControlPoint*> constControlPoints() const override;
        virtual QPointF pointAtPercent(qreal t) const override;
        virtual QRectF controlPointRect() const override;

        virtual ~CubicBezier() {}

    protected:
        virtual void paintPathItemImpl(QPainterPath *totalPainterPath, QPainter *painter,
                                       bool editable, const PathSettings *settings) const override;

    private:
        qshared_ptr<ControlPoint> _cPoint1;
        qshared_ptr<ControlPoint> _cPoint2;

        QList<qshared_ptr<ControlPoint> > _controlPoints;
    };
}

#endif // CUBICBEZIER_H
