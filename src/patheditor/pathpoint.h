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

#ifndef PATHPOINT_H
#define PATHPOINT_H

#include "patheditorfwd/patheditorfwd.h"

#include <QSharedPointer>
#include <QPen>
#include "restrictor.h"
#include "pointhandle.h"

namespace patheditor
{
    class PathPoint : public QObject, public QPointF
    {
        Q_OBJECT
    public:
        explicit PathPoint();
        explicit PathPoint(qreal xpos, qreal ypos);

        virtual void setParent(QObject *object);

        void setRestrictedX(qreal xpos);
        void setRestrictedY(qreal ypos);
        void setRestrictedPos(qreal xpos, qreal ypos);

        void setRestrictor(QSharedPointer<Restrictor> restrictor);

        /**
         * Creates a PointHandle and adds it to the passed scene.
         * When overriding, use replaceCurrentPointHandle(PointHandle *pointHandle)
         * to replace the current PointHandle.
         *
         * @param settings The PathSettings to use
         * @param scene The QGraphicsScene to add the PointHandle to
         */
        virtual void createPointHandle(PathSettings &settings, QGraphicsItem *parent);

        void addFollowingPoint(QSharedPointer<PathPoint> point);

        bool visible();

        void select();

        bool selected() const;

        PointHandle *handle();

        virtual ~PathPoint() {}

    signals:
        void pointDrag(PathPoint *sender);
        void pointRelease(PathPoint *sender);

    public slots:
        void onPointDrag(QGraphicsSceneMouseEvent *event);
        void onPointRelease(QGraphicsSceneMouseEvent *event);

    protected:
        /**
         * Replaces the current PointHandle and takes ownership of the passed pointHandle
         *
         * @param pointHandle PointHandle to replace the current handle with
         */
        void replaceCurrentPointHandle(PointHandle *pointHandle);

    private:
        void setPos(qreal xpos, qreal ypos);

        bool _selected;
        static void select(PathPoint *point, QGraphicsScene *scene);

        QSharedPointer<Restrictor> _restrictor;
        PathPoint* _toFollowPoint;
        QList<QWeakPointer<PathPoint> > _followingPoints;

        PointHandle *_pointHandle;

    };
}

#endif // PATHPOINT_H
