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

#include "thicknesscontours.h"

#include <QPainter>
#include <QtAlgorithms>
#include <QGraphicsScene>
#include "foilcalculator.h"
#include "foil.h"

using namespace foileditors;
using namespace foillogic;

ThicknessContours::ThicknessContours(foillogic::FoilCalculator *calculator, Side::e side, QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    _side = side;
    _nextDetailed = false;
    _calculator = calculator;
}

void ThicknessContours::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*unused*/, QWidget * /*unused*/)
{
    if (_calculator->calculated())
    {
        int min = 0;
        int max = 255;
        int a = 100;
        int increment = 0;

        painter->setBrush(QColor(min, 0, max, a));

        QList<QSharedPointer<QPainterPath> > contours = _calculator->calculatedContours(_side);

        int numberOfContours = contours.length();
        if (numberOfContours > 0)
        {
            increment = (max - min) / numberOfContours;
            foreach (const QSharedPointer<QPainterPath>& contour, contours)
            {
                min += increment;
                max -= increment;

                painter->setBrush(QColor(min, 0, max, a));
                painter->drawPath(*contour);
            }
        }
    }
}

QRectF ThicknessContours::boundingRect() const
{
    if (!_calculator->calculated())
    {
        QRectF retVal;
        return retVal;
    }

    return _calculator->foil()->outline()->controlPointRect();
}
