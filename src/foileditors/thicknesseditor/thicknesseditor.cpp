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

#include "foileditors/thicknesseditor/thicknesseditor.hpp"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QGraphicsScene>
#include "patheditor/patheditorwidget.hpp"
#include "patheditor/editablepath.hpp"
#include "foillogic/foil.hpp"
#include "foillogic/thicknessprofile.hpp"

using namespace foileditors;
using namespace foillogic;

ThicknessEditor::ThicknessEditor(QWidget *parent) :
    QWidget(parent)
{
    _pathEditor = new patheditor::PathEditorWidget();
    _pathEditor->enableFeatures(QFlags<Features::e>(Features::HorizontalAxis | Features::VerticalAxis |
                                                    Features::DragImageHereText));

    _modeCombo = new QComboBox();
    _modeCombo->addItem(tr("Thickness"));
    _modeCombo->addItem(tr("Aspect Ratio"));
    connect(_modeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(modeChanged(int)));

    QGroupBox* gb = new QGroupBox(tr("Thickness"));
    QVBoxLayout* gbLayout = new QVBoxLayout();
    gbLayout->addWidget(_modeCombo);
    gbLayout->addWidget(_pathEditor);
    gb->setLayout(gbLayout);

    _mainLayout = new QVBoxLayout();
    _mainLayout->addWidget(gb);
    this->setLayout(_mainLayout);
}

void ThicknessEditor::setFoil(Foil *foil)
{
    _modeCombo->setCurrentIndex(foil->thicknessProfile()->aspectRatioEnforced()? 1 : 0);
    _pathEditor->clear();

    EditablePath* botPath = new EditablePath(foil->thicknessProfile()->botProfile());
    botPath->setEditable(false);
    connect(foil->thicknessProfile(), SIGNAL(mirrored()), this, SLOT(update()), Qt::UniqueConnection); // A non-editable path won't signal updates to it's scene
    EditablePath* topPath = new EditablePath(foil->thicknessProfile()->topProfile());
    topPath->setEditable(foil->thicknessProfile()->editable());

    _pathEditor->addPath(topPath);
    _pathEditor->addPath(botPath);

    _foil = foil;
    _topPath = topPath;
    _botPath = botPath;
}

void ThicknessEditor::update()
{
    _pathEditor->scene()->update();
}

void ThicknessEditor::setImage(const QString &path)
{
  _pathEditor->setImage(path);
}

void ThicknessEditor::modeChanged(int mode)
{
  switch (mode) {
  case 0: // Thickness
    _foil->thicknessProfile()->setAspectRatioEnforced(false);
    setFoil(_foil);
    break;
  case 1: // Aspect Ratio
    _foil->thicknessProfile()->setAspectRatioEnforced(true);
    setFoil(_foil);
    break;
  }

  bool editable = true; // _foil->thicknessProfile()->editable();
  _botPath->setEditable(editable && false);
  _topPath->setEditable(editable && true);

  _pathEditor->scene()->invalidate();
}
