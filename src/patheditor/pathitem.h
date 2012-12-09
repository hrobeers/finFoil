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

#include <QGraphicsItem>
#include <QList>

namespace patheditor
{
    class PathItem : public QGraphicsItem
    {
    public:
        explicit PathItem(QSharedPointer<QPointF> startPoint, QSharedPointer<QPointF> endPoint,
                          QGraphicsItem * parent = 0, QGraphicsScene * scene = 0);

        // Variables needed by EditablePath for editing
        QSharedPointer<QPointF> startPoint;
        QSharedPointer<QPointF> endPoint;

        // Functions needed by EditablePath for editing
        virtual int numberOfControlPoints() const = 0;
        virtual QList<QSharedPointer<QPointF> > controlPoints() = 0;

        // Implementing QGraphicsItem
        virtual QRectF boundingRect() const = 0;
        virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
                        QWidget *widget) = 0;
    };
}

#endif // PATHITEM_H
