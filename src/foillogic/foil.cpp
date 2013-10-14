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

#include "foil.h"

#include "pathpoint.h"
#include "editablepath.h"
#include "linerestrictor.h"
#include "quadrantrestrictor.h"
#include "cubicbezier.h"
#include "profile.h"

using namespace foillogic;
using namespace patheditor;

Foil::Foil(QObject *parent) :
    QObject(parent)
{
    initOutline();
    initProfile();
    initThickness();

    connect(_outline.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));
    connect(_profile.data(), SIGNAL(profileChanged(Profile*)), this, SLOT(onFoilChanged()));
    connect(_thickness.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));

    connect(_outline.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
    connect(_profile.data(), SIGNAL(profileReleased(Profile*)), this, SLOT(onFoilReleased()));
    connect(_thickness.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
}

QSharedPointer<Path> Foil::outline()
{
    return _outline;
}

QSharedPointer<Profile> Foil::profile()
{
    return _profile;
}

QSharedPointer<Path> Foil::thickness()
{
    return _thickness;
}

Foil::~Foil()
{
}

void Foil::initOutline()
{
    _outline = QSharedPointer<Path>(new Path());

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

    QSharedPointer<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point13));
    QSharedPointer<Restrictor> aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    point1->setRestrictor(horizontalAxisRestrictor);
    point4->setRestrictor(aboveHorizontalRestrictor);
    point7->setRestrictor(aboveHorizontalRestrictor);
    point10->setRestrictor(aboveHorizontalRestrictor);
    point13->setRestrictor(horizontalAxisRestrictor);

    _outline->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    _outline->append(QSharedPointer<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    _outline->append(QSharedPointer<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    _outline->append(QSharedPointer<PathItem>(new CubicBezier(point10, point11, point12, point13)));
}

void Foil::initProfile()
{
    _profile = QSharedPointer<Profile>(new Profile());
}

void Foil::initThickness()
{
    _thickness = QSharedPointer<Path>(new Path());

    QSharedPointer<PathPoint> point0(new PathPoint(0,0));
    QSharedPointer<PathPoint> point1(new PathPoint(0,-30));
    QSharedPointer<ControlPoint> point2(new ControlPoint(0,-30));
    QSharedPointer<ControlPoint> point3(new ControlPoint(200,-30));
    QSharedPointer<PathPoint> point4(new PathPoint(200,0));

    QSharedPointer<Restrictor> verticalAxisRestrictor(new LineRestrictor(*point0, *point1));
    QSharedPointer<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point0, *point4));

    point1->setRestrictor(verticalAxisRestrictor);
    point4->setRestrictor(horizontalAxisRestrictor);

    _thickness->append(QSharedPointer<PathItem>(new CubicBezier(point1, point2, point3, point4)));
}

void Foil::onFoilChanged()
{
    emit foilChanged(this);
}

void Foil::onFoilReleased()
{
    emit foilReleased(this);
}
