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

#include "patheditorview.h"

using namespace patheditor;

PathEditorView::PathEditorView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
}

void PathEditorView::drawBackground(QPainter *painter, const QRectF &rect)
{
    QPen gridPen(QColor(0, 0, 0, 100), 1, Qt::SolidLine);
    painter->setPen(gridPen);

    QPointF bottomLeft = rect.bottomLeft();
    QPointF topRight = rect.topRight();

    for (qreal h = 0; h > topRight.y(); h -= 10)
        painter->drawLine(bottomLeft.x(), h, topRight.x(), h);
    for (qreal h = 10; h < bottomLeft.y(); h += 10)
        painter->drawLine(bottomLeft.x(), h, topRight.x(), h);

    for (qreal w = 0; w < topRight.x(); w += 10)
        painter->drawLine(w, bottomLeft.y(), w, topRight.y());
    for (qreal w = -10; w > bottomLeft.x(); w -= 10)
        painter->drawLine(w, bottomLeft.y(), w, topRight.y());
}
