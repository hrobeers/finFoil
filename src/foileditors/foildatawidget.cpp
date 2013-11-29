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

#include "foildatawidget.h"

#include <qmath.h>
#include <QFormLayout>
#include <QGroupBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "editablepath.h"
#include "foilcalculator.h"
#include "foillogic/foil.h"
#include "foillogic/profile.h"

#include "hrlib/units/area.h"

using namespace foileditors;
using namespace foillogic;
using namespace hrlib::units;

FoilDataWidget::FoilDataWidget(foillogic::FoilCalculator *foilCalculator, QWidget *parent) :
    QWidget(parent)
{
    _foilCalculator = foilCalculator;
    _formLayout = new QFormLayout();

    connect(_foilCalculator, SIGNAL(foilCalculated(FoilCalculator*)), this, SLOT(onFoilCalculated()));


    //
    // Layer section
    //
    _layerEdit = new QSpinBox();
    _layerEdit->setValue(_foilCalculator->contourThicknesses().count() + 1);
    _formLayout->addRow(tr("#Layers:"), _layerEdit);
    connect(_layerEdit, SIGNAL(valueChanged(int)), this, SLOT(onLayerChange(int)));


    //
    // Depth section
    //
    _depth = _foilCalculator->foil()->height();
    _depthEdit = new UnitDoubleSpinbox<Length>();
//    _depthEdit->setMaximum(10000);
    Length depth(_depth, Length::Unit::cm);
    _depthEdit->setValue(depth);
    _formLayout->addRow(tr("Depth:"), _depthEdit);
    connect(_depthEdit, SIGNAL(valueChanged(hrlib::units::IQuantity*)), this, SLOT(onDepthChange(hrlib::units::IQuantity*)));


    //
    // Area section
    //
    _areaEdit = new UnitLineEdit<Area>();
    _areaEdit->setReadOnly(true);
    _formLayout->addRow(tr("Area:"), _areaEdit);


    //
    // Sweep section
    //
    _sweepEdit = new UnitLineEdit<Angle>();
    _sweepEdit->setReadOnly(true);
    _formLayout->addRow(tr("Sweep:"), _sweepEdit);


    //
    // Thickness ratio section
    //
    _thicknessRatioEdit = new QLineEdit(thicknessRatioString(_foilCalculator->foil()->profile()->thicknessRatio()));
    _thicknessRatioEdit->setReadOnly(true);
    _formLayout->addRow(tr("Thickness ratio:"), _thicknessRatioEdit);


    QGroupBox* gb = new QGroupBox(tr("Fin Properties"));
    gb->setLayout(_formLayout);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(gb);

    this->setLayout(layout);
}


void FoilDataWidget::onDepthChange(IQuantity *depth)
{
    Length* ldepth = static_cast<Length*>(depth);
    _depth = ldepth->internalValue();
    _foilCalculator->foil()->setHeight(_depth);
    _foilCalculator->recalculateArea();
    onFoilCalculated();
}

void FoilDataWidget::updatePxPerUnit()
{
    if (_depth.value() != 0)
    {
        _pxPerUnit = -_foilCalculator->foil()->outline()->minY() / _depth.value();
    }
    else
    {
        _pxPerUnit = 0;
    }

    emit pxPerUnitChanged(_pxPerUnit);
}

void FoilDataWidget::updateArea()
{
    if (_pxPerUnit != 0)
    {
        Area area(_foilCalculator->foil()->area(), Area::Unit::cm2);
        _areaEdit->setValue(area);
    }
}

QString FoilDataWidget::thicknessRatioString(qreal ratio)
{
    qreal bot = qRound(100 / (1 + ratio));
    qreal top = 100 - bot;
    return QString::number(top) + "/" + QString::number(bot);
}

void FoilDataWidget::onFoilCalculated()
{
    updatePxPerUnit();
    updateArea();
    Angle angle(_foilCalculator->foil()->sweep(), Angle::Unit::degree);
    _sweepEdit->setValue(angle);
    _thicknessRatioEdit->setText(thicknessRatioString(_foilCalculator->foil()->profile()->thicknessRatio()));
}

void FoilDataWidget::onLayerChange(int layerCount)
{
    qreal increment = qreal(1) / qreal(layerCount);
    qreal thickness = 0;
    QList<qreal> thicknesses;
    for (int i = 0; i < layerCount - 1; i++)
    {
        thickness += increment;
        thicknesses.append(thickness);
    }

    qSort(thicknesses);
    _foilCalculator->setContourThicknesses(thicknesses);
}
