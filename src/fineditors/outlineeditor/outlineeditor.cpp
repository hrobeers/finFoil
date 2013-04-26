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

    qreal m = 2;
    QSharedPointer<PathPoint> point1(new PathPoint(m*0, m*0));
    QSharedPointer<ControlPoint> point2(new ControlPoint(m*16.09549195, m*-31.53267));
    QSharedPointer<ControlPoint> point3(new ControlPoint(m*70.39944295, m*-113.577872));
    QSharedPointer<PathPoint> point4(new PathPoint(m*134.750359, m*-114.484482));
    QSharedPointer<ControlPoint> point5(new ControlPoint(m*148.079229, m*-114.672267));
    QSharedPointer<ControlPoint> point6(new ControlPoint(m*168.493739, m*-110.447322));
    QSharedPointer<PathPoint> point7(new PathPoint(m*170.304549, m*-97.240702));
    QSharedPointer<ControlPoint> point8(new ControlPoint(m*171.482419, m*-88.650189));
    QSharedPointer<ControlPoint> point9(new ControlPoint(m*134.604629, m*-78.11541));
    QSharedPointer<PathPoint> point10(new PathPoint(m*123.550789, m*-62.04205));
    QSharedPointer<ControlPoint> point11(new ControlPoint(m*99.87859895, m*-27.6204));
    QSharedPointer<ControlPoint> point12(new ControlPoint(m*116.439959, m*0));
    QSharedPointer<PathPoint> point13(new PathPoint(m*116.439959, m*0));

    QSharedPointer<Restrictor> _aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    point1->setRestrictor(_pathEditor->horizontalAxisRestrictor());
    point4->setRestrictor(_aboveHorizontalRestrictor);
    point7->setRestrictor(_aboveHorizontalRestrictor);
    point10->setRestrictor(_aboveHorizontalRestrictor);
    point13->setRestrictor(_pathEditor->horizontalAxisRestrictor());

    EditablePath* path = new EditablePath();
    path->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    path->append(QSharedPointer<PathItem>(new CubicBezier(point10, point11, point12, point13)));

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
