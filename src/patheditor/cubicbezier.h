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

#include <QPainter>
#include "pathitem.h"

namespace patheditor
{
    class CubicBezier : public PathItem
    {
    public:
        explicit CubicBezier(QSharedPointer<QPointF> startPoint, QSharedPointer<QPointF> endPoint,
                             QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

        QSharedPointer<QPointF> controlPoint1();
        QSharedPointer<QPointF> controlPoint2();

        // implementing PathItem
        QList<QSharedPointer<QPointF> > controlPoints();

        // Implementing QGraphicsItem
        QRectF boundingRect() const;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget);

    private:
        QSharedPointer<QPointF> _cPoint1;
        QSharedPointer<QPointF> _cPoint2;

        QList<QSharedPointer<QPointF> > _controlPoints;

        QRectF _boundingRect;
    };
}

#endif // CUBICBEZIER_H
