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

#include "foil.h"
#include "contourcalculator.h"

using namespace foillogic;

FoilCalculator::FoilCalculator(Foil *foil) :
    QObject()
{
    _calculated = false;
    _foil = foil;

    connect(_foil, SIGNAL(foilChanged(Foil*)), this, SLOT(foilChanged()));
}

Foil *FoilCalculator::foil()
{
    return _foil;
}

void FoilCalculator::setContourThicknesses(QList<qreal> thicknesses)
{
    _contourThicknesses = thicknesses;
}

QList<QSharedPointer<QPainterPath> > FoilCalculator::calculatedContours()
{
    return _contours;
}

void FoilCalculator::calculate()
{
    _contours.clear();
//    if (profilesSet())
//    {
#ifdef SERIAL
        foreach (qreal thickness, _contourThicknesses)
        {
            QSharedPointer<QPainterPath> path(new QPainterPath());
            _contours.append(path);

            ContourCalculator calc(thickness, _foil->outline().data(), _foil->profile().data(),
                                   _foil->thickness().data(), path.data(), false);
            calc.run();
        }
#endif
#ifndef SERIAL
        foreach (qreal thickness, _contourThicknesses)
        {
            QSharedPointer<QPainterPath> path(new QPainterPath());
            _contours.append(path);

            _tPool.start(new ContourCalculator(thickness, _foil->outline().data(), _foil->profile().data(),
                                               _foil->thickness().data(), path.data(), true));
        }
        _tPool.waitForDone();
#endif
//    }

    _calculated = true;
}

bool FoilCalculator::calculated()
{
    return _calculated;
}

void FoilCalculator::foilChanged()
{
    calculate();
}
