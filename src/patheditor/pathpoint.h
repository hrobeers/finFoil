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

#ifndef RESTRICTABLEPOINT_H
#define RESTRICTABLEPOINT_H

#include <QPointF>
#include <QSharedPointer>
#include "restrictor.h"
#include "pointhandle.h"

namespace patheditor
{
    // Forward declarations
    class PathSettings;

    // Enum Structs
    struct PointType
    {
        enum e
        {
            Point,
            ControlPoint
        };
    };

    class PathPoint : public QPointF
    {
    public:
        explicit PathPoint(qreal xpos, qreal ypos, PointType::e type = PointType::Point);

        void setRestrictedPos(qreal xpos, qreal ypos);

        void setRestrictor(QSharedPointer<Restrictor> &restrictor);

        void createPointHandle(PathSettings &settings, QGraphicsItem *parent, QGraphicsScene *scene);

        void addLinkedPoint(QWeakPointer<PathPoint> linkedPoint);

    private:
        void setPos(qreal xpos, qreal ypos);

        PointType::e _type;

        QSharedPointer<Restrictor> _restrictor;
        QList<QWeakPointer<PathPoint> > _linkedPoints;

        PointHandle *_pointHandle;
    };
}

#endif // RESTRICTABLEPOINT_H
