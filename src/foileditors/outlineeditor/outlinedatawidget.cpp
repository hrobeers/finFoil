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

#include <qmath.h>
#include <QFormLayout>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QLineEdit>

#include "editablepath.h"

using namespace foileditors;

OutlineDataWidget::OutlineDataWidget(QWidget *parent) :
    QWidget(parent)
{
    _depth = 0;
    _outlinePtr = 0;

    _formLayout = new QFormLayout();


    //
    // Depth section
    //
    _depthEdit = new QDoubleSpinBox();
    _depthEdit->setMaximum(10000);
    _formLayout->addRow(tr("Depth:"), _depthEdit);
    connect(_depthEdit, SIGNAL(valueChanged(double)), this, SLOT(onDepthChange(double)));


    //
    // Area section
    //
    _areaEdit = new QLineEdit("0");
    _areaEdit->setReadOnly(true);
    _formLayout->addRow(tr("Area:"), _areaEdit);

    QGroupBox* gb = new QGroupBox(tr("Fin Properties"));
    gb->setLayout(_formLayout);
    QVBoxLayout* layout = new QVBoxLayout();
    layout->addWidget(gb);

    this->setLayout(layout);
}


void OutlineDataWidget::onDepthChange(double depth)
{
    _depth = depth;
    emit depthChanged((qreal)depth);

    if (_outlinePtr != 0)
        onOutlineChange(_outlinePtr);
}

void OutlineDataWidget::onOutlineChange(EditablePath *sender)
{
    onAreaChange(sender->area(500), sender);
}

void OutlineDataWidget::onAreaChange(qreal area, EditablePath *sender)
{
    if (_depth != 0)
    {
        _pxPerUnit = -sender->minY() / _depth;
        _areaEdit->setText(QString::number(area / (_pxPerUnit * _pxPerUnit)));
    }
    else
    {
        _pxPerUnit = 0;
        _areaEdit->setText("0");
    }

    _outlinePtr = sender;

    emit pxPerUnitChanged(_pxPerUnit);
}
