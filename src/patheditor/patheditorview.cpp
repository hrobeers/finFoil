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

#define MIN_UNIT_SIZE 5

using namespace patheditor;

PathEditorView::PathEditorView(QGraphicsScene *scene, QWidget *parent) :
    QGraphicsView(scene, parent)
{
    _unitSize = 10;
}

void PathEditorView::setUnitSize(qreal pxPerUnit)
{
    if (pxPerUnit == 0)
        _unitSize = 10;
    else
        _unitSize = pxPerUnit;

    scene()->update(); // TODO fix double update (or move contour calculation out of paint method)
}

void PathEditorView::drawBackground(QPainter *painter, const QRectF &rect)
{
        qreal unitSize = _unitSize;
        while (unitSize < MIN_UNIT_SIZE)
            unitSize *= 10;

        painter->setPen(QPen(QColor(0, 0, 0, 50), 1, Qt::SolidLine));
        drawLinesWithInterval(unitSize, painter, rect);

        painter->setPen(QPen(QColor(0, 0, 0, 100), 1, Qt::SolidLine));
        drawLinesWithInterval(unitSize*10, painter, rect);
}

void PathEditorView::drawLinesWithInterval(qreal px, QPainter *painter, const QRectF &rect)
{
    QVector<QPointF> pointPairs;

    QPointF bottomLeft = rect.bottomLeft();
    QPointF topRight = rect.topRight();

    for (qreal h = 0; h > topRight.y(); h -= px)
    {
        pointPairs.append(QPointF(bottomLeft.x(), h));
        pointPairs.append(QPointF(topRight.x(), h));
    }
    for (qreal h = px; h < bottomLeft.y(); h += px)
    {
        pointPairs.append(QPointF(bottomLeft.x(), h));
        pointPairs.append(QPointF(topRight.x(), h));
    }

    for (qreal w = 0; w < topRight.x(); w += px)
    {
        pointPairs.append(QPointF(w, bottomLeft.y()));
        pointPairs.append(QPointF(w, topRight.y()));
    }
    for (qreal w = -px; w > bottomLeft.x(); w -= px)
    {
        pointPairs.append(QPointF(w, bottomLeft.y()));
        pointPairs.append(QPointF(w, topRight.y()));
    }

    painter->drawLines(pointPairs);
}
