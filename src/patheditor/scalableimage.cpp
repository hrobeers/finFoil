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
#include "pathsettings.h"
#include <QPainter>
#include "qmath.h"

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

    PathSettings settings = PathSettings::Default();
    _scalePoint->createPointHandle(settings, this);
}

void ScalableImage::paint(QPainter *painter, const QStyleOptionGraphicsItem */*unused*/, QWidget */*unused*/)
{
    painter->drawPixmap(_rect, _pixmap);
}

QRectF ScalableImage::boundingRect() const
{
    return QRectF(_rect);
}

ScalableImage::~ScalableImage()
{
}
