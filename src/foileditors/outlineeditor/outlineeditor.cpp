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

#include "outlineeditor.h"

#include <QVBoxLayout>
#include <QGroupBox>
#include "patheditor/patheditorwidget.h"
#include "foil.h"
#include "editablepath.h"
#include "thicknesscontours.h"

using namespace foileditors;
using namespace foillogic;

OutlineEditor::OutlineEditor(Foil *foil, QWidget *parent) :
    QWidget(parent)
{
    //
    // PathEditor
    //
    _pathEditor = new patheditor::PathEditorWidget();
    _pathEditor->enableFeature(Features::HorizontalAxis);

    EditablePath* path = foil->outline();

    ThicknessContours *contours = new ThicknessContours();
    connect(path, SIGNAL(pathChanged(EditablePath*)), contours, SLOT(onOutlineChange(EditablePath*)));
    connect(this, SIGNAL(profileChanged(EditablePath*)), contours, SLOT(onProfileChange(EditablePath*)));
    connect(this, SIGNAL(thicknessChanged(EditablePath*)), contours, SLOT(onThicknessChange(EditablePath*)));

    _pathEditor->addGraphicsItem(contours);
    _pathEditor->addPath(path);


    //
    // OutlineDataWidget
    //
    _outlineDataWidget = new OutlineDataWidget();
    connect(path, SIGNAL(pathChanged(EditablePath*)), _outlineDataWidget, SLOT(onOutlineChange(EditablePath*)));
    connect(_outlineDataWidget, SIGNAL(pxPerUnitChanged(qreal)), _pathEditor, SLOT(setGridUnitSize(qreal)));


    //
    // Layout
    //
    QGroupBox* gb = new QGroupBox(tr("Outline Editor"));
    QVBoxLayout* gbLayout = new QVBoxLayout();
    gbLayout->addWidget(_pathEditor);
    gbLayout->addWidget(_outlineDataWidget);
    gb->setLayout(gbLayout);

    _mainLayout = new QVBoxLayout();
    _mainLayout->addWidget(gb);
    this->setLayout(_mainLayout);
}

void OutlineEditor::onProfileChange(EditablePath *sender)
{
    emit profileChanged(sender);
}

void OutlineEditor::onThicknessChange(EditablePath *sender)
{
    emit thicknessChanged(sender);
}
