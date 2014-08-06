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

#include "scalableimage.h"
#include <QPainter>
#include <QGraphicsScene>
#include "qmath.h"
#include "pathsettings.h"
#include "linerestrictor.h"
#include "pointhandle.h"
#include "scalepoint.h"

using namespace patheditor;

ScalableImage::ScalableImage(const QPixmap &pixmap, const QRect &initialRect, QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    _pixmap = pixmap;
    _rect = initialRect;

    qreal imageAR = (qreal)pixmap.height() / (qreal)pixmap.width();
    qreal rectAR = (qreal)_rect.height() / (qreal)_rect.width();
    if (imageAR > rectAR)
        _rect.setWidth(_rect.height() / imageAR);
    else
        _rect.setHeight(_rect.width() * imageAR);


    _scalePoint.reset(new ScalePoint(_rect.topRight().x(), _rect.topRight().y()));

    // TODO make configurable
    const PathSettings *settings = PathSettings::Default();
    _scalePoint->createPointHandle(this, settings);

    connect(_scalePoint.get(), SIGNAL(pointDrag(PathPoint*)), this, SLOT(onScaleMove(PathPoint*)));
    connect(_scalePoint.get(), SIGNAL(pointRelease(PathPoint*)), this, SLOT(onScaleMove(PathPoint*)));

    QPointF botLeft(_rect.bottomLeft());
    QPointF topRight(_rect.topRight());
    std::shared_ptr<LineRestrictor> restrictor(new LineRestrictor(botLeft, topRight));
    _scalePoint->setRestrictor(restrictor);
}

void ScalableImage::paint(QPainter *painter, const QStyleOptionGraphicsItem */*unused*/, QWidget */*unused*/)
{
    painter->drawPixmap(_rect, _pixmap);
}

QRectF ScalableImage::boundingRect() const
{
    return QRectF(_rect) | _scalePoint.get()->handle()->boundingRect();
}

ScalableImage::~ScalableImage() {}

void ScalableImage::onScaleMove(PathPoint *point)
{
    QRect oldRect = _rect;

    QPointF relativePnt = *point - _rect.bottomLeft();

    _rect.setHeight(-relativePnt.y());
    _rect.setWidth(relativePnt.x());

    _rect.moveBottomLeft(oldRect.bottomLeft());

    this->scene()->update();
}
