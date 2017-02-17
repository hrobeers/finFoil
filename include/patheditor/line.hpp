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

#ifndef LINE_HPP
#define LINE_HPP

#include "patheditor/fwd/patheditorfwd.hpp"

#include "patheditor/pathitem.hpp"

namespace patheditor
{
    /**
     * @brief The Line PathItem
     */
    class Line : public PathItem
    {
    public:
        explicit Line(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint);

        // implementing PathItem
        virtual QList<std::shared_ptr<ControlPoint> > controlPoints() override;
        virtual const QList<const ControlPoint*> constControlPoints() const override;
        virtual QPointF pointAtPercent(qreal t) const override;
        virtual QRectF controlPointRect() const override;

        virtual ~Line() {}

    protected:
        virtual void paintPathItemImpl(QPainterPath *totalPainterPath, QPainter *painter,
                                       bool editable, const PathSettings *settings) const override;
    };
}

#endif // LINE_HPP
