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
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "patheditor/path.hpp"
#include "patheditor/editablepath.hpp"
#include "foillogic/foilcalculator.hpp"
#include "foillogic/foil.hpp"
#include "foillogic/profile.hpp"
#include "foillogic/outline.hpp"

#include <QtUnits>

using namespace foileditors;
using namespace foillogic;
using namespace qt::units;

FoilDataWidget::FoilDataWidget(QWidget *parent) :
    QWidget(parent), _pxPerUnitOutline(0), _pxPerUnitProfile(0)
{
    QFormLayout* formLayoutLeft = new QFormLayout();
    QFormLayout* formLayoutRight = new QFormLayout();
    formLayoutLeft->setFormAlignment(Qt::AlignBottom);
    formLayoutRight->setFormAlignment(Qt::AlignBottom);


    //
    // Unit selection section
    //
    _unitSelector = new QComboBox();
    _unitSelector->addItem("m");
    _unitSelector->addItem("cm");
    _unitSelector->addItem("ft");
    _unitSelector->addItem("inch");
    _unitSelector->setCurrentIndex(1);
    formLayoutLeft->addRow(tr("Unit system"), _unitSelector);
    connect(_unitSelector, SIGNAL(currentIndexChanged(QString)), this, SLOT(onUnitSystemChange(QString)));


    //
    // Layer section
    //
    _layerEdit = new QSpinBox();
    formLayoutLeft->addRow(tr("#Layers:"), _layerEdit);
    connect(_layerEdit, SIGNAL(valueChanged(int)), this, SLOT(onLayerChange(int)));


    //
    // Depth section
    //
    _depthEdit = new UnitDoubleSpinbox<Length>();
    formLayoutLeft->addRow(tr("Depth:"), _depthEdit);
    connect(_depthEdit, SIGNAL(valueChanged(qt::units::IQuantity*)), this, SLOT(onDepthChange(qt::units::IQuantity*)));


    //
    // Thickness section
    //
    _thicknessEdit = new UnitDoubleSpinbox<Length>();
    formLayoutLeft->addRow(tr("Thickness:"), _thicknessEdit);
    connect(_thicknessEdit, SIGNAL(valueChanged(qt::units::IQuantity*)), this, SLOT(onThicknessChange(qt::units::IQuantity*)));


    //
    // Minimum thickness section
    //
    _minThickEdit = new UnitDoubleSpinbox<Length>();
    formLayoutRight->addRow(tr("Min thickness:"), _minThickEdit);
    connect(_minThickEdit, SIGNAL(valueChanged(qt::units::IQuantity*)), this, SLOT(onMinThickChange(qt::units::IQuantity*)));


    //
    // Area section
    //
    _areaEdit = new UnitLineEdit<Area>();
    _areaEdit->setReadOnly(true);
    formLayoutRight->addRow(tr("Area:"), _areaEdit);


    //
    // Sweep section
    //
    _sweep.setUnit(AngleUnit::degree);
    _sweepEdit = new UnitLineEdit<Angle>();
    _sweepEdit->setReadOnly(true);
    formLayoutRight->addRow(tr("Sweep:"), _sweepEdit);


    //
    // Thickness ratio section
    //
    _thicknessRatioEdit = new QLineEdit();
    _thicknessRatioEdit->setReadOnly(true);
    formLayoutRight->addRow(tr("Thickness ratio:"), _thicknessRatioEdit);


    QGroupBox* gb = new QGroupBox(tr("Fin Properties"));
    QHBoxLayout* formContainer = new QHBoxLayout();
    formContainer->addLayout(formLayoutLeft, 1);
    formContainer->addSpacing(5);
    formContainer->addLayout(formLayoutRight, 1);
    gb->setLayout(formContainer);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(gb);

    this->setLayout(layout);
}

void FoilDataWidget::setFoilCalculator(FoilCalculator *foilCalculator)
{
    _foilCalculator = foilCalculator;
    connect(_foilCalculator, SIGNAL(foilCalculated(FoilCalculator*)), this, SLOT(onFoilCalculated()));

    _layerEdit->setValue(_foilCalculator->contourThicknesses().count() + 1);

    _depth.setInternalValue(_foilCalculator->foil()->outline()->height());
    _depthEdit->setValue(_depth);

    _thickness.setInternalValue(_foilCalculator->foil()->thickness());
    _thicknessEdit->setValue(_thickness);

    _minThick.setInternalValue(_foilCalculator->foil()->minThickness());
    _minThickEdit->setValue(_minThick);

    _thicknessRatioEdit->setText(thicknessRatioString(_foilCalculator->foil()->profile()->thicknessRatio()));
}

void FoilDataWidget::showEvent(QShowEvent *)
{
    onUnitSystemChange(_unitSelector->currentText(), true);
}

void FoilDataWidget::updatePxPerUnit()
{
    _pxPerUnitOutline = qAbs(_foilCalculator->foil()->outline()->path()->minY() / _depthEdit->value()->value());
    _pxPerUnitProfile = qAbs(_foilCalculator->foil()->profile()->pxThickness() / _thicknessEdit->value()->value());

    emit pxPerUnitOutlineChanged(_pxPerUnitOutline);
    emit pxPerUnitProfileChanged(_pxPerUnitProfile);
}

void FoilDataWidget::updateArea()
{
    if (_pxPerUnitOutline != 0)
    {
        _area.setInternalValue(_foilCalculator->foil()->outline()->area());
        _areaEdit->setValue(_area);
    }
}

QString FoilDataWidget::thicknessRatioString(qreal ratio)
{
    qreal bot = qRound(100 / (1 + ratio));
    qreal top = 100 - bot;
    return QString::number(top) + "/" + QString::number(bot);
}

void FoilDataWidget::setLengthUnits(LengthUnit lengthUnit)
{
    _depth.setUnit(lengthUnit);
    _thickness.setUnit(lengthUnit);
    _minThick.setUnit(lengthUnit);
}

void FoilDataWidget::onFoilCalculated()
{
    updatePxPerUnit();
    updateArea();
    _sweep.setInternalValue(_foilCalculator->foil()->outline()->sweep());
    _sweepEdit->setValue(_sweep);
    _thicknessRatioEdit->setText(thicknessRatioString(_foilCalculator->foil()->profile()->thicknessRatio()));
}

void FoilDataWidget::onLayerChange(int layerCount)
{
    _foilCalculator->setEquidistantContours(layerCount);
}

void FoilDataWidget::onUnitSystemChange(const QString &system, bool showEvent)
{
    if (system == "m")
    {
        setLengthUnits(LengthUnit::m);
        _area.setUnit(AreaUnit::m2);
    }
    else if (system == "cm")
    {
        setLengthUnits(LengthUnit::cm);
        _area.setUnit(AreaUnit::cm2);
    }
    else if (system == "ft")
    {
        setLengthUnits(LengthUnit::ft);
        _area.setUnit(AreaUnit::ft2);
    }
    else if (system == "inch")
    {
        setLengthUnits(LengthUnit::inch);
        _area.setUnit(AreaUnit::inch2);
    }

    updateArea();
    _depthEdit->setValue(_depth, !showEvent);
    _thicknessEdit->setValue(_thickness, !showEvent);
    _minThickEdit->setValue(_minThick, !showEvent);
}

void FoilDataWidget::onDepthChange(IQuantity *depth)
{
    Length* ldepth = static_cast<Length*>(depth);
    _depth.setInternalValue(ldepth->internalValue());
    _foilCalculator->foil()->outline()->setHeight(_depth.internalValue());
    _foilCalculator->recalculateArea();
    onFoilCalculated();
    emit depthChanged(&_depth);
}

void FoilDataWidget::onThicknessChange(IQuantity *thickness)
{
    Length* lthickness = static_cast<Length*>(thickness);
    _thickness.setInternalValue(lthickness->internalValue());
    _foilCalculator->foil()->setThickness(_thickness.internalValue());
    updatePxPerUnit();
    emit thicknessChanged(&_thickness);
}

void FoilDataWidget::onMinThickChange(qt::units::IQuantity *minThick)
{
  Length* lminThick = static_cast<Length*>(minThick);
  _minThick.setInternalValue(lminThick->internalValue());
  _foilCalculator->foil()->setMinThickness(_minThick.internalValue());
}
