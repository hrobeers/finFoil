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

#include <QVBoxLayout>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QLineEdit>

#include "editablepath.h"

using namespace fineditors;

OutlineDataWidget::OutlineDataWidget(QWidget *parent) :
    QWidget(parent)
{
    _vLayout = new QVBoxLayout();


    //
    // Height section
    //
    QHBoxLayout* heightLayout = new QHBoxLayout();
    QLabel* heightLabel = new QLabel(tr("Fin height:"));
    _heightEdit = new QDoubleSpinBox();
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(_heightEdit);
    _vLayout->addLayout(heightLayout);
    connect(_heightEdit, SIGNAL(valueChanged(double)), this, SLOT(onHeightChange(double)));


    //
    // Area section
    //
    QHBoxLayout* areaLayout = new QHBoxLayout();
    QLabel* areaLabel = new QLabel(tr("Fin area:"));
    _areaEdit = new QLineEdit("0");
    areaLayout->addWidget(areaLabel);
    areaLayout->addWidget(_areaEdit);
    _vLayout->addLayout(areaLayout);

    this->setLayout(_vLayout);
}


void OutlineDataWidget::onHeightChange(double height)
{
    emit heightChanged((qreal)height);
}

void OutlineDataWidget::onOutlineChange(EditablePath *sender)
{
    _areaEdit->setText(QString::number(sender->area(2000)));
}
