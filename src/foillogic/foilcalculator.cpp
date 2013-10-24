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

#include "foilcalculator.h"

#include "qmath.h"
#include "foil.h"
#include "profile.h"
#include "contourcalculator.h"

using namespace foillogic;

FoilCalculator::FoilCalculator(Foil *foil) :
    QObject()
{
    _calculated = false;
    _foil = foil;

    int numOfContours = 6;
    qreal increment = qreal(1) / qreal(numOfContours);
    qreal thickness = 0;
    QList<qreal> thicknesses;
    for (int i = 0; i < numOfContours - 1; i++)
    {
        thickness += increment;
        thicknesses.append(thickness);
    }

    qSort(thicknesses);
    setContourThicknesses(thicknesses);

    connect(_foil, SIGNAL(foilChanged(Foil*)), this, SLOT(foilChanged()));
    connect(_foil, SIGNAL(foilReleased(Foil*)), this, SLOT(foilReleased()));
}

Foil *FoilCalculator::foil()
{
    return _foil;
}

QList<qreal> FoilCalculator::contourThicknesses() const
{
    return _contourThicknesses;
}

void FoilCalculator::setContourThicknesses(QList<qreal> thicknesses)
{
    _contourThicknesses = thicknesses;
    calculate(false);
}

QList<QSharedPointer<QPainterPath> > FoilCalculator::topContours()
{
    return _topContours;
}

QList<QSharedPointer<QPainterPath> > FoilCalculator::bottomContours()
{
    return _botContours;
}

void FoilCalculator::calculate(bool fastCalc)
{
    _topContours.clear();
    _botContours.clear();
#ifdef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        QSharedPointer<QPainterPath> topPath(new QPainterPath());
        _contours.append(topPath);
        ContourCalculator tcCalc(thickness, _foil, topPath.data(), Side::Top, fastCalc);


        QSharedPointer<QPainterPath> botPath(new QPainterPath());
        _botContours.append(botPath);
        ContourCalculator bcCalc(thickness, _foil, botPath.data(), Side::Bottom, fastCalc);

        AreaCalculator aCalc(_foil, &_area);
        SweepCalculator sCalc(_foil, &_sweep);

        tcCalc.run();
        bcCalc.run();
        aCalc.run();
        sCalc.run();
    }
#endif
#ifndef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        QSharedPointer<QPainterPath> topPath(new QPainterPath());
        _topContours.append(topPath);
        _tPool.start(new ContourCalculator(thickness, _foil, topPath.data(), Side::Top, fastCalc));

        QSharedPointer<QPainterPath> botPath(new QPainterPath());
        _botContours.append(botPath);
        _tPool.start(new ContourCalculator(thickness, _foil, botPath.data(), Side::Bottom, fastCalc));
    }

    _tPool.start(new AreaCalculator(_foil, &_area));
    _tPool.start(new SweepCalculator(_foil, &_sweep));

    _tPool.waitForDone();
#endif

    _calculated = true;
    emit foilCalculated(this);
}

bool FoilCalculator::calculated() const
{
    return _calculated;
}

qreal FoilCalculator::area() const
{
    return _area;
}

qreal FoilCalculator::sweep() const
{
    return _sweep;
}

void FoilCalculator::foilChanged()
{
    calculate(true);
}

void FoilCalculator::foilReleased()
{
    calculate(false);
}


AreaCalculator::AreaCalculator(Foil *foil, qreal *area)
{
    _foil = foil;
    _area = area;
}

void AreaCalculator::run()
{
    *_area = _foil->outline()->area();
}


SweepCalculator::SweepCalculator(Foil *foil, qreal *sweep)
{
    _foil = foil;
    _sweep = sweep;
}

void SweepCalculator::run()
{
    // find top and outline edges
    qreal t_top = 0;
    _foil->outline()->minY(&t_top);
    QPointF top = _foil->outline()->pointAtPercent(t_top);
    qreal oLEdge = _foil->outline()->pointAtPercent(0).x();
    qreal oTEdge = _foil->outline()->pointAtPercent(1).x();

    // find thickest point
    qreal t_thick = 0;
    _foil->profile()->topProfile()->minY(&t_thick);
    qreal thick = _foil->profile()->topProfile()->pointAtPercent(t_thick).x();
    qreal pEdge = _foil->profile()->topProfile()->pointAtPercent(1).x();
    qreal thickX = thick/pEdge * (oTEdge - oLEdge) + oLEdge;

    // calculate the sweep angle in degrees
    qreal os = top.x() - thickX;
    qreal ns = -top.y();
    *_sweep = qAtan(os/ns) * 180 / M_PI;
}
