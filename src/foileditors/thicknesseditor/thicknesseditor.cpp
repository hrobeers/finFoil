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

#include "thicknesseditor.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QGraphicsScene>
#include "patheditorwidget.h"
#include "editablepath.h"
#include "foil.h"
#include "thicknessprofile.h"

using namespace foileditors;
using namespace foillogic;

ThicknessEditor::ThicknessEditor(QWidget *parent) :
    QWidget(parent)
{
    _pathEditor = new patheditor::PathEditorWidget();
    _pathEditor->enableFeatures(QFlags<Features::e>(Features::HorizontalAxis | Features::VerticalAxis |
                                                    Features::DragImageHereText));

    QGroupBox* gb = new QGroupBox(tr("Thickness"));
    QVBoxLayout* gbLayout = new QVBoxLayout();
    gbLayout->addWidget(_pathEditor);
    gb->setLayout(gbLayout);

    _mainLayout = new QVBoxLayout();
    _mainLayout->addWidget(gb);
    this->setLayout(_mainLayout);
}

void ThicknessEditor::setFoil(Foil *foil)
{
    _pathEditor->clear();

    EditablePath* botPath = new EditablePath(foil->thickness()->botProfile());
    botPath->setEditable(false);
    connect(foil->thickness(), SIGNAL(mirrored()), this, SLOT(update())); // A non-editable path won't signal updates to it's scene
    EditablePath* topPath = new EditablePath(foil->thickness()->topProfile());

    _pathEditor->addPath(topPath);
    _pathEditor->addPath(botPath);
}

void ThicknessEditor::update()
{
    _pathEditor->scene()->update();
}
