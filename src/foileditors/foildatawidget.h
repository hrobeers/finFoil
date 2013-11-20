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

#ifndef FOILDATAWIDGET_H
#define FOILDATAWIDGET_H

#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"
#include "foillogicfwd/foillogicfwd.h"

#include <QWidget>
#include <boost/units/quantity.hpp>
#include <boost/units/systems/si/length.hpp>
#include "hrlib/units/widgets/unitlineedit.h" //TODO forward declare

using namespace patheditor;

namespace foileditors
{
    class FoilDataWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit FoilDataWidget(foillogic::FoilCalculator* foilCalculator, QWidget *parent = 0);

    signals:
        void pxPerUnitChanged(qreal pxPerUnit);

    public slots:
        void onDepthChange(double depth);

    private:
        boost::units::quantity<boost::units::si::length, qreal> _depth;
        qreal _pxPerUnit;
        foillogic::FoilCalculator* _foilCalculator;

        QFormLayout* _formLayout;
        QSpinBox* _layerEdit;
        QDoubleSpinBox* _depthEdit;
        hrlib::units::UnitLineEdit* _areaEdit;
        QLineEdit* _sweepEdit;
        QLineEdit* _thicknessRatioEdit;

        void updatePxPerUnit();
        void updateArea();
        QString thicknessRatioString(qreal ratio);

    private slots:
        void onFoilCalculated();
        void onLayerChange(int layerCount);
    };
}

#endif // FOILDATAWIDGET_H
