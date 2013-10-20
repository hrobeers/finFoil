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

using namespace foileditors;
using namespace foillogic;

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
    _layerEdit->setValue(_foilCalculator->contourThicknesses().count());
    _formLayout->addRow(tr("#Layers:"), _layerEdit);
    connect(_layerEdit, SIGNAL(valueChanged(int)), this, SLOT(onLayerChange(int)));


    //
    // Depth section
    //
    _depth = 3;
    _depthEdit = new QDoubleSpinBox();
    _depthEdit->setMaximum(10000);
    _depthEdit->setValue(_depth);
    _formLayout->addRow(tr("Depth:"), _depthEdit);
    connect(_depthEdit, SIGNAL(valueChanged(double)), this, SLOT(onDepthChange(double)));


    //
    // Area section
    //
    _areaEdit = new QLineEdit("0");
    _areaEdit->setReadOnly(true);
    _formLayout->addRow(tr("Area:"), _areaEdit);


    //
    // Sweep section
    //
    _sweepEdit = new QLineEdit("0");
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


void FoilDataWidget::onDepthChange(double depth)
{
    _depth = depth;
    emit depthChanged((qreal)depth);

    AreaChanged(_foilCalculator->area());
}

void FoilDataWidget::AreaChanged(qreal area)
{
    if (_depth != 0)
    {
        _pxPerUnit = -_foilCalculator->foil()->outline()->minY() / _depth;
        _areaEdit->setText(QString::number(area / (_pxPerUnit * _pxPerUnit)));
    }
    else
    {
        _pxPerUnit = 0;
        _areaEdit->setText("0");
    }

    emit pxPerUnitChanged(_pxPerUnit);
}

QString FoilDataWidget::thicknessRatioString(qreal ratio)
{
    qreal bot = qRound(100 / (1 + ratio));
    qreal top = 100 - bot;
    return QString::number(top) + "/" + QString::number(bot);
}

void FoilDataWidget::onFoilCalculated()
{
    AreaChanged(_foilCalculator->area());
    _sweepEdit->setText(QString::number(_foilCalculator->sweep()));
    _thicknessRatioEdit->setText(thicknessRatioString(_foilCalculator->foil()->profile()->thicknessRatio()));
}

void FoilDataWidget::onLayerChange(int layerCount)
{
    qreal increment = qreal(1) / qreal(layerCount);
    qreal thickness = 0;
    QList<qreal> thicknesses;
    for (int i = 0; i < layerCount; i++)
    {
        thickness += increment;
        thicknesses.append(thickness);
    }

    qSort(thicknesses);
    _foilCalculator->setContourThicknesses(thicknesses);
}
