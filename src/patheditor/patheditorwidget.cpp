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

#include "patheditorwidget.h"

#include <QGraphicsView>
#include <QLayout>
#include "linerestrictor.h"
#include "pointrestrictor.h"
#include "editablepath.h"

#define featureEnabled(feat) (_enabledFeatures & feat)

using namespace patheditor;

PathEditorWidget::PathEditorWidget(QWidget *parent) :
    QWidget(parent)
{
    // init member variables
    _scene = new QGraphicsScene(this);
    _view = new QGraphicsView(_scene, this);
    _mainLayout = new QVBoxLayout(this);

    _enabledFeatures = Features::None;

    QPointF origin(0,0);
    _originRestrictor = QSharedPointer<Restrictor>(new PointRestrictor(origin));

    QPointF right(1,0);
    _horizontalAxisRestrictor = QSharedPointer<Restrictor>(new LineRestrictor(origin, right));

    QPointF under(0,1);
    _verticalAxisRestrictor = QSharedPointer<Restrictor>(new LineRestrictor(origin, under));

    // view options
    _view->setRenderHint(QPainter::Antialiasing);

    // layout
    _mainLayout->addWidget(_view);
    this->setLayout(_mainLayout);

    connect(this->scene(), SIGNAL(sceneRectChanged(QRectF)), this, SLOT(onSceneRectChanged(QRectF)));
}

QGraphicsScene *PathEditorWidget::scene()
{
    return _scene;
}

void PathEditorWidget::addPath(EditablePath* path)
{
    this->scene()->addItem(path);
}

void PathEditorWidget::addGraphicsItem(QGraphicsItem *item)
{
    this->scene()->addItem(item);
}

void PathEditorWidget::enableFeature(Features::e feature)
{
    if (!featureEnabled(feature))
    {
        switch (feature)
        {
        case Features::HorizontalAxis:
            _horizontalAxis = new QGraphicsLineItem(this->scene()->sceneRect().left(), 0,
                                                    this->scene()->sceneRect().right(), 0);
            this->scene()->addItem(_horizontalAxis);
            break;

        case Features::VerticalAxis:
            _verticalAxis = new QGraphicsLineItem(0, this->scene()->sceneRect().bottom(),
                                                  0, this->scene()->sceneRect().top());
            this->scene()->addItem(_verticalAxis);
            break;

        case Features::None:
            break;
        }

        _enabledFeatures = _enabledFeatures | feature;
    }
}

QSharedPointer<Restrictor> PathEditorWidget::originRestrictor()
{
    return _originRestrictor;
}

QSharedPointer<Restrictor> PathEditorWidget::horizontalAxisRestrictor()
{
    return _horizontalAxisRestrictor;
}

QSharedPointer<Restrictor> PathEditorWidget::verticalAxisRestrictor()
{
    return _verticalAxisRestrictor;
}

void PathEditorWidget::onSceneRectChanged(const QRectF &rect)
{
    if (featureEnabled(Features::HorizontalAxis))
        _horizontalAxis->setLine(rect.left(), 0, rect.right(), 0);
    if (featureEnabled(Features::VerticalAxis))
        _verticalAxis->setLine(0, rect.bottom(), 0, rect.top());
}
