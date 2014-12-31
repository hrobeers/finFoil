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

#ifndef PATH_H
#define PATH_H

#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"

#include <QObject>
#include <memory>
#include "ipath.hpp"

#define PATH_AREARES 512

namespace patheditor
{
    class Path : public QObject, public IPath
    {
        Q_OBJECT

    public:
        explicit Path(QObject *parent = 0);

        /**
         * @brief append Append a new path item to the path
         * @param pathItem PathItem to append
         */
        virtual void append(std::shared_ptr<PathItem> pathItem);

        QList<std::shared_ptr<PathItem> > pathItems();
        QList<const PathItem *> constPathItems() const;

        // TODO unittest methods below

        QRectF controlPointRect() const;

        virtual QPointF pointAtPercent(qreal t) const override;

        virtual qreal minX(qreal *t_top = 0) const override;
        virtual qreal maxX(qreal *t_top = 0) const override;
        virtual qreal minY(qreal *t_top = 0) const override;
        virtual qreal maxY(qreal *t_top = 0) const override;

        qreal area(int resolution = PATH_AREARES) const;

        void paint(QPainter *painter, bool editable = false, const PathSettings *settings = 0);

        virtual ~Path() {}

    signals:
        void onAppend(patheditor::PathItem *pathItem);
        void pathChanged(patheditor::Path *sender);
        void pathReleased(patheditor::Path *sender);

    public slots:
        void onPathChanged();
        void onPathReleased();

    private:
        QList<std::shared_ptr<PathItem> > _pathItemList;
    };
}

#endif // PATH_H
