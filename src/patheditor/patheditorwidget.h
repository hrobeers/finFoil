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

#ifndef PATHEDITORWIDGET_H
#define PATHEDITORWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QLayout>
#include "editablepath.h"

namespace patheditor
{
    struct Features
    {
        enum e
        {
            None = 0,
            HorizontalAxis = 1,
            VerticalAxis = 2
        };
    };

    class PathEditorWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit PathEditorWidget(QWidget *parent = 0);

        /**
         * Returns a pointer to the QGraphicsScene used in this PathEditorWidget
         */
        QGraphicsScene* scene();

        /**
         * Adds an EditablePath object to this widget.
         * This widget takes ownership of the item.
         *
         * @param path The EditablePath to add
         */
        void addPath(EditablePath *path);

        /**
         * Enables a specific feature
         *
         * @param feature to enable
         */
        void enableFeature(Features::e feature);

    signals:

    private slots:
        void onSceneRectChanged ( const QRectF & rect );

    private:
        QGraphicsView* _view;
        QGraphicsScene* _scene;
        QVBoxLayout* _mainLayout;


        // Features
        int _enabledFeatures;
        QGraphicsLineItem* _horizontalAxis;
        QGraphicsLineItem* _verticalAxis;
    };
}

#endif // PATHEDITORWIDGET_H
