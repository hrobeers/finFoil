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
#include <QtUnits>

using namespace patheditor;

namespace foileditors
{
    class FoilDataWidget : public QWidget
    {
        Q_OBJECT
    public:
        explicit FoilDataWidget(QWidget *parent = 0);

        void setFoilCalculator(foillogic::FoilCalculator* foilCalculator);

        virtual ~FoilDataWidget() {}

    signals:
        void depthChanged(qt::units::Length *height);
        void thicknessChanged(qt::units::Length *thickness);

        void pxPerUnitOutlineChanged(qreal pxPerUnit);
        void pxPerUnitProfileChanged(qreal pxPerUnit);

    protected:
        virtual void showEvent(QShowEvent *);

    private:
        qt::units::Length _depth;
        qt::units::Length _thickness;
        qt::units::Length _minThick;
        qt::units::Area _area;
        qt::units::Angle _sweep;

        qreal _pxPerUnitOutline;
        qreal _pxPerUnitProfile;
        foillogic::FoilCalculator* _foilCalculator;

        QComboBox* _unitSelector;
        QSpinBox* _layerEdit;
        qt::units::UnitDoubleSpinbox<qt::units::Length>* _depthEdit;
        qt::units::UnitDoubleSpinbox<qt::units::Length>* _thicknessEdit;
        qt::units::UnitDoubleSpinbox<qt::units::Length>* _minThickEdit;
        qt::units::UnitLineEdit<qt::units::Area>* _areaEdit;
        qt::units::UnitLineEdit<qt::units::Angle>* _sweepEdit;
        QLineEdit* _thicknessRatioEdit;

        void updatePxPerUnit();
        void updateArea();
        QString thicknessRatioString(qreal ratio);

        void setLengthUnits(qt::units::LengthUnit lengthUnit);

    private slots:
        void onFoilCalculated();
        void onLayerChange(int layerCount);
        void onUnitSystemChange(const QString &system);
        void onDepthChange(qt::units::IQuantity *depth);
        void onThicknessChange(qt::units::IQuantity *thickness);
        void onMinThickChange(qt::units::IQuantity *minThick);
    };
}

#endif // FOILDATAWIDGET_H
