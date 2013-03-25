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
#include "editablepath.h"
#include "cubicbezier.h"
#include "line.h"
#include "quadrantrestrictor.h"
#include "thicknesscontours.h"

using namespace fineditors;

OutlineEditor::OutlineEditor(QWidget *parent) :
    QWidget(parent)
{
    _pathEditor = new patheditor::PathEditorWidget(this);
    _pathEditor->enableFeature(Features::HorizontalAxis);

    QSharedPointer<PathPoint> point1(new PathPoint(0,0));
    QSharedPointer<PathPoint> point2(new PathPoint(40,-100));
    QSharedPointer<PathPoint> point3(new PathPoint(100,-200));
    QSharedPointer<PathPoint> point4(new PathPoint(150,-100));
    QSharedPointer<PathPoint> point5(new PathPoint(120,0));

    QSharedPointer<Restrictor> _aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    point1->setRestrictor(_pathEditor->horizontalAxisRestrictor());
    point2->setRestrictor(_aboveHorizontalRestrictor);
    point3->setRestrictor(_aboveHorizontalRestrictor);
    point4->setRestrictor(_aboveHorizontalRestrictor);
    point5->setRestrictor(_pathEditor->horizontalAxisRestrictor());

    EditablePath* path = new EditablePath();
    path->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point2, point3)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point3, point4)));
    path->append(QSharedPointer<PathItem>(new Line(point4, point5)));

    ThicknessContours *contours = new ThicknessContours();
    connect(path, SIGNAL(pathChanged(EditablePath*)), contours, SLOT(onOutlineChange(EditablePath*)));
    connect(this, SIGNAL(profileChanged(EditablePath*)), contours, SLOT(onProfileChange(EditablePath*)));
    connect(this, SIGNAL(thicknessChanged(EditablePath*)), contours, SLOT(onThicknessChange(EditablePath*)));

    _pathEditor->addGraphicsItem(contours);
    _pathEditor->addPath(path);

    _mainLayout = new QVBoxLayout(this);
    _mainLayout->addWidget(_pathEditor);
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
