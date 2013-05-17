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
#include "editablepath.h"
#include "cubicbezier.h"
#include "line.h"

using namespace fineditors;

ThicknessEditor::ThicknessEditor(QWidget *parent) :
    QWidget(parent)
{
    _pathEditor = new patheditor::PathEditorWidget();
    _pathEditor->enableFeature(Features::HorizontalAxis);
    _pathEditor->enableFeature(Features::VerticalAxis);

    QSharedPointer<PathPoint> point1(new PathPoint(0,-30));
    QSharedPointer<ControlPoint> point2(new ControlPoint(0,-30));
    QSharedPointer<ControlPoint> point3(new ControlPoint(200,-30));
    QSharedPointer<PathPoint> point4(new PathPoint(200,0));

    point1->setRestrictor(_pathEditor->verticalAxisRestrictor());
    point4->setRestrictor(_pathEditor->horizontalAxisRestrictor());

    EditablePath* path = new EditablePath();
    path->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    // Pipe the pathchanged signal
    connect(path, SIGNAL(pathChanged(EditablePath*)), this, SIGNAL(thicknessChanged(EditablePath*)));

    _pathEditor->addPath(path);

    QGroupBox* gb = new QGroupBox(tr("Thickness Editor"));
    QVBoxLayout* gbLayout = new QVBoxLayout();
    gbLayout->addWidget(_pathEditor);
    gb->setLayout(gbLayout);

    _mainLayout = new QVBoxLayout();
    _mainLayout->addWidget(gb);
    this->setLayout(_mainLayout);
}

ThicknessEditor::~ThicknessEditor()
{
}
