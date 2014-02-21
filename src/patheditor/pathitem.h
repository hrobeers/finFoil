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

#ifndef PATHITEM_H
#define PATHITEM_H

#include "patheditorfwd/patheditorfwd.h"

#include "pathpoint.h"
#include "controlpoint.h"

namespace patheditor
{
    class PathItem
    {
    public:
        PathItem(std::shared_ptr<PathPoint> startPoint, std::shared_ptr<PathPoint> endPoint);


        //
        // Functions needed by EditablePath
        //
        virtual std::shared_ptr<PathPoint> startPoint();
        virtual std::shared_ptr<PathPoint> endPoint();
        virtual void setStartPoint(std::shared_ptr<PathPoint> startPoint);
        virtual void setEndPoint(std::shared_ptr<PathPoint> endPoint);

        virtual QList<std::shared_ptr<ControlPoint> > controlPoints() = 0;

        virtual std::weak_ptr<PathItem> nextPathItem();
        virtual std::weak_ptr<PathItem> prevPathItem();
        virtual void setNextPathItem(std::shared_ptr<PathItem> nextPathItem);
        virtual void setPrevPathItem(std::shared_ptr<PathItem> prevPathItem);

        virtual QRectF controlPointRect() const = 0;

        //
        // EditablePath paint methods
        //
        void paintControlPoints(PathSettings *settings, QPainter *painter);
        virtual void paintPathItem(PathSettings *settings, QPainterPath *totalPainterPath, QPainter *painter,
                           const QStyleOptionGraphicsItem *option, QWidget *widget, bool editable = true) = 0;

        //
        // Custom path calculation (preferably faster than using QPainterPath)
        //
        virtual QPointF pointAtPercent(qreal t) = 0;

        virtual ~PathItem() {}

    protected:
        std::shared_ptr<PathPoint> _startPoint;
        std::shared_ptr<PathPoint> _endPoint;

    private:
        std::weak_ptr<PathItem> _nextPathItem;
        std::weak_ptr<PathItem> _prevPathItem;
    };
}

#endif // PATHITEM_H
