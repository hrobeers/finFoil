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

#ifndef PATHITEM_HPP
#define PATHITEM_HPP

#include "hrlib/fwd/qtfwd.hpp"
#include "patheditor/fwd/patheditorfwd.hpp"

#include <memory>
#include <QList>
#include <QPointF>

namespace patheditor
{
    class PathItem
    {
    public:
        //
        // Functions needed by EditablePath
        //
        std::shared_ptr<PathPoint> startPoint();
        std::shared_ptr<PathPoint> endPoint();
        void setStartPoint(std::shared_ptr<PathPoint> startPoint);
        void setEndPoint(std::shared_ptr<PathPoint> endPoint);

        virtual QList<std::shared_ptr<ControlPoint> > controlPoints() = 0;
        QList<QPointF> points() const;

        // const getters
        const PathPoint* constStartPoint() const;
        const PathPoint* constEndPoint() const;
        virtual const QList<const ControlPoint*> constControlPoints() const = 0;

        virtual std::weak_ptr<PathItem> nextPathItem() const;
        virtual std::weak_ptr<PathItem> prevPathItem() const;
        virtual void setNextPathItem(std::shared_ptr<PathItem> nextPathItem);
        virtual void setPrevPathItem(std::shared_ptr<PathItem> prevPathItem);

        virtual QRectF controlPointRect() const = 0;

        virtual std::shared_ptr<PathItem> clone() const = 0;

        //
        // EditablePath paint methods
        //
        void paintControlPoints(QPainter *painter, const PathSettings *settings = 0) const;
        void paintPathItem(QPainterPath *totalPainterPath, QPainter *painter,
                           bool editable = true, const PathSettings *settings = 0) const;

        //
        // Custom path calculation (preferably faster than using QPainterPath)
        //
        virtual QPointF pointAtPercent(qreal t) const = 0;

        virtual ~PathItem() {}

    protected:
        std::shared_ptr<PathPoint> _startPoint;
        std::shared_ptr<PathPoint> _endPoint;

        virtual void paintPathItemImpl(QPainterPath *totalPainterPath, QPainter *painter,
                                       bool editable, const PathSettings *settings) const = 0;

    private:
        std::weak_ptr<PathItem> _nextPathItem;
        std::weak_ptr<PathItem> _prevPathItem;
    };
}

#endif // PATHITEM_HPP
