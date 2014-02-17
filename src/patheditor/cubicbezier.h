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
        Q_OBJECT

    public:
        Q_INVOKABLE CubicBezier() {}

        explicit CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint);

        explicit CubicBezier(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<ControlPoint> controlPoint1,
                             std::shared_ptr<ControlPoint> controlPoint2, std::shared_ptr<PathPoint> endPoint);

        std::shared_ptr<ControlPoint> controlPoint1();
        std::shared_ptr<ControlPoint> controlPoint2();

        // implementing PathItem
        QList<std::shared_ptr<ControlPoint> > controlPoints();
        virtual QPointF pointAtPercent(qreal t);
        QRectF controlPointRect() const;

        void paintPathItem(PathSettings *settings, QPainterPath *totalPainterPath, QPainter *painter,
                   const QStyleOptionGraphicsItem *option, QWidget *widget, bool editable = true);

        virtual ~CubicBezier() {}

    private:
        std::shared_ptr<ControlPoint> _cPoint1;
        std::shared_ptr<ControlPoint> _cPoint2;

        QList<std::shared_ptr<ControlPoint> > _controlPoints;
    };
}
DESERIALIZABLE(patheditor::CubicBezier, cubic)

#endif // CUBICBEZIER_H
