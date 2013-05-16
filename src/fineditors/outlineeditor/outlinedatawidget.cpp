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

#include "outlinedatawidget.h"

#include <cmath>
#include <qmath.h>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "editablepath.h"

using namespace fineditors;

OutlineDataWidget::OutlineDataWidget(QWidget *parent) :
    QWidget(parent)
{
    _height = 0;
    _outlinePtr = 0;

    _layout = new QFormLayout();


    //
    // Height section
    //
    _heightEdit = new QDoubleSpinBox();
    _heightEdit->setMaximum(10000);
    _layout->addRow(tr("Height:"), _heightEdit);
    connect(_heightEdit, SIGNAL(valueChanged(double)), this, SLOT(onHeightChange(double)));


    //
    // Area section
    //
    _areaEdit = new QLineEdit("0");
    _areaEdit->setReadOnly(true);
    _layout->addRow(tr("Area:"), _areaEdit);

    this->setLayout(_layout);
}


void OutlineDataWidget::onHeightChange(double height)
{
    _height = height;
    emit heightChanged((qreal)height);

    if (_outlinePtr != 0)
        onOutlineChange(_outlinePtr);
}

void OutlineDataWidget::onOutlineChange(EditablePath *sender)
{
    onAreaChange(sender->area(500), sender);
}

void OutlineDataWidget::onAreaChange(qreal area, EditablePath *sender)
{
    if (_height != 0)
    {
        qreal scale = -sender->minY() / _height;
        _areaEdit->setText(QString::number(area / (scale * scale)));
    }
    else
        _areaEdit->setText("0");

    _outlinePtr = sender;
}
