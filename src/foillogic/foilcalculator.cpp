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
#include "contourcalculator.h"

using namespace foillogic;

FoilCalculator::FoilCalculator(Foil *foil) :
    QObject()
{
    _calculated = false;
    _foil = foil;

    int numOfContours = 4;
    qreal increment = qreal(1) / qreal(numOfContours+1);
    qreal thickness = 0;
    QList<qreal> thicknesses;
    thicknesses.append(0);
    for (int i = 0; i < numOfContours; i++)
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

QList<qreal> FoilCalculator::contourThicknesses()
{
    return _contourThicknesses;
}

void FoilCalculator::setContourThicknesses(QList<qreal> thicknesses)
{
    _contourThicknesses = thicknesses;
    calculate(false);
}

QList<QSharedPointer<QPainterPath> > FoilCalculator::calculatedContours()
{
    return _contours;
}

void FoilCalculator::calculate(bool fastCalc)
{
    _contours.clear();
#ifdef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        QSharedPointer<QPainterPath> path(new QPainterPath());
        _contours.append(path);

        ContourCalculator cCalc(thickness, _foil->outline().data(), _foil->topProfile().data(),
                               _foil->thickness().data(), path.data(), fastCalc);
        AreaCalculator aCalc(_foil, &_area);
        SweepCalculator sCalc(_foil, &_sweep);

        cCalc.run();
        aCalc.run();
        sCalc.run();
    }
#endif
#ifndef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        QSharedPointer<QPainterPath> path(new QPainterPath());
        _contours.append(path);

        _tPool.start(new ContourCalculator(thickness, _foil->outline().data(), _foil->topProfile().data(),
                                           _foil->thickness().data(), path.data(), fastCalc));
    }

    _tPool.start(new AreaCalculator(_foil, &_area));
    _tPool.start(new SweepCalculator(_foil, &_sweep));

    _tPool.waitForDone();
#endif

    _calculated = true;
    emit foilCalculated(this);
}

bool FoilCalculator::calculated()
{
    return _calculated;
}

qreal FoilCalculator::area()
{
    return _area;
}

qreal FoilCalculator::sweep()
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
    _foil->topProfile()->minY(&t_thick);
    qreal thick = _foil->topProfile()->pointAtPercent(t_thick).x();
    qreal pEdge = _foil->topProfile()->pointAtPercent(1).x();
    qreal thickX = thick/pEdge * (oTEdge - oLEdge) + oLEdge;

    // calculate the sweep angle in degrees
    qreal os = top.x() - thickX;
    qreal ns = -top.y();
    *_sweep = qAtan(os/ns) * 180 / M_PI;
}
