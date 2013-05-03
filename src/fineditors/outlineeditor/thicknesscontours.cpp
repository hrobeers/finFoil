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
#include "editablepath.h"
#include "contourcalculator.h"

using namespace fineditors;

ThicknessContours::ThicknessContours(QGraphicsItem *parent) :
    QGraphicsObject(parent)
{
    _nextDetailed = false;

    int numOfContours = std::max(_tPool.maxThreadCount(), 3);
    qreal increment = qreal(1) / qreal(numOfContours+1);
    qreal thickness = 0;
    for (int i = 0; i < numOfContours; i++)
    {
        thickness += increment;
        _contourThicknesses.append(thickness);
    }

    qSort(_contourThicknesses);
}

void ThicknessContours::paint(QPainter *painter, const QStyleOptionGraphicsItem * /*unused*/, QWidget * /*unused*/)
{
    if (!_outline.isNull())
    {
        int min = 0;
        int max = 255;
        int a = 100;
        int increment = 0;

        painter->setBrush(QColor(min, 0, max, a));
        painter->drawPath(*_outline);

        int numberOfContours = _contours.length();
        if (numberOfContours > 0)
        {
            increment = (max - min) / numberOfContours;
            foreach (const QSharedPointer<QPainterPath>& contour, _contours)
            {
                min += increment;
                max -= increment;

                painter->setBrush(QColor(min, 0, max, a));
                painter->drawPath(*contour);
            }
        }

        calcContours();
    }
}

QRectF ThicknessContours::boundingRect() const
{
    if (_outline.isNull())
    {
        QRectF retVal;
        return retVal;
    }

    return _outline->boundingRect();
}

void ThicknessContours::onOutlineChange(EditablePath *sender)
{
    _outline = sender->painterPath();
}

void ThicknessContours::onProfileChange(EditablePath *sender)
{
    _profile = sender->painterPath();
    this->scene()->update(boundingRect());
}

void ThicknessContours::onThicknessChange(EditablePath *sender)
{
    _thickness = sender->painterPath();
    this->scene()->update(boundingRect());
}

void ThicknessContours::calcContours()
{
    _contours.clear();
    if (profilesSet())
    {
#ifdef PROCEDURAL
        foreach (qreal thickness, _contourThicknesses)
        {
            QSharedPointer<QPainterPath> path(new QPainterPath());
            _contours.append(path);

            ContourCalculator calc(thickness, _outline.data(), _profile.data(), _thickness.data(), path.data(), true);
            calc.run();
        }
#endif
#ifndef PROCEDURAL
        foreach (qreal thickness, _contourThicknesses)
        {
            QSharedPointer<QPainterPath> path(new QPainterPath());
            _contours.append(path);

            _tPool.start(new ContourCalculator(thickness, _outline.data(), _profile.data(), _thickness.data(), path.data(), true));
        }
        _tPool.waitForDone();
#endif
    }
}

bool ThicknessContours::profilesSet()
{
    if (_outline.isNull())
        return false;

    if (_profile.isNull())
        return false;

    if (_thickness.isNull())
        return false;

    return true;
}
