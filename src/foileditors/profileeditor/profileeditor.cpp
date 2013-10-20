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

#include "profileeditor.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QComboBox>
#include <QGraphicsScene>
#include "editablepath.h"
#include "foil.h"
#include "profile.h"

using namespace foileditors;
using namespace foillogic;

ProfileEditor::ProfileEditor(QSharedPointer<Foil> foil, QWidget *parent) :
    QWidget(parent)
{
    _foil = foil;

    _pathEditor = new patheditor::PathEditorWidget();
    _pathEditor->enableFeature(Features::HorizontalAxis);

    _topProfile = new EditablePath(_foil->profile()->topProfile());
    _botProfile = new EditablePath(_foil->profile()->botProfile());
    symmetryChanged(0);

    _pathEditor->addPath(_botProfile);
    _pathEditor->addPath(_topProfile);

    QComboBox* symmetryCombo = new QComboBox();
    symmetryCombo->addItem(tr("Symmetric"));
    symmetryCombo->addItem(tr("Asymmetric"));
    symmetryCombo->addItem(tr("Flat"));
    connect(symmetryCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(symmetryChanged(int)));

    QGroupBox* gb = new QGroupBox(tr("Profile"));
    QVBoxLayout* gbLayout = new QVBoxLayout();
    gbLayout->addWidget(symmetryCombo);
    gbLayout->addWidget(_pathEditor);
    gb->setLayout(gbLayout);

    _mainLayout = new QVBoxLayout();
    _mainLayout->addWidget(gb);
    this->setLayout(_mainLayout);
}

ProfileEditor::~ProfileEditor()
{
}

void ProfileEditor::symmetryChanged(int sym)
{
    switch (sym)
    {
    case 0:
        _foil->profile()->setSymmetry(Symmetry::Symmetric);
        _botProfile->setEditable(false);
        _topProfile->setEditable(true);
        break;
    case 1:
        _foil->profile()->setSymmetry(Symmetry::Asymmetric);
        _botProfile->setEditable(true);
        break;
    case 2:
        _foil->profile()->setSymmetry(Symmetry::Flat);
        _botProfile->setEditable(false);
        _topProfile->setEditable(true);
        break;
    }

    _pathEditor->scene()->invalidate();
}
