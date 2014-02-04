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
#include "thicknessprofile.h"

using namespace foillogic;
using namespace patheditor;
using namespace boost::units;

Foil::Foil(QObject *parent) :
    QObject(parent)
{
    initOutline();
    initProfile();
    initThickness();

    _height = quantity<si::length, qreal>(0.1 * si::meter); // 10cm
    _area = quantity<si::area, qreal>(0 * si::meter * si::meter);

    connect(_outline.get(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));
    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onFoilChanged()));
    connect(_thickness.get(), SIGNAL(profileChanged(ThicknessProfile*)), this, SLOT(onFoilChanged()));

    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onProfileChanged()));

    connect(_outline.get(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
    connect(_profile.get(), SIGNAL(profileReleased(Profile*)), this, SLOT(onFoilReleased()));
    connect(_thickness.get(), SIGNAL(profileReleased(ThicknessProfile*)), this, SLOT(onFoilReleased()));
}

std::shared_ptr<Path> Foil::outline()
{
    return _outline;
}

std::shared_ptr<Profile> Foil::profile()
{
    return _profile;
}

std::shared_ptr<ThicknessProfile> Foil::thickness()
{
    return _thickness;
}

quantity<si::length, qreal> Foil::height()
{
    return _height;
}

quantity<si::area, qreal> Foil::area()
{
    return _area;
}

boost::units::quantity<si::plane_angle, qreal> Foil::sweep()
{
    return _sweep;
}

void Foil::setHeight(quantity<si::length, qreal> height)
{
    _height = height;
}

void Foil::setArea(quantity<si::area, qreal> area)
{
    _area = area;
}

void Foil::setSweep(boost::units::quantity<si::plane_angle, qreal> sweep)
{
    _sweep = sweep;
}

Foil::~Foil()
{
}

void Foil::initOutline()
{
    _outline = std::shared_ptr<Path>(new Path());

    qreal m = 2;
    std::shared_ptr<PathPoint> point1(new PathPoint(m*0, m*0));
    std::shared_ptr<ControlPoint> point2(new ControlPoint(m*16.09549195, m*-31.53267));
    std::shared_ptr<ControlPoint> point3(new ControlPoint(m*70.39944295, m*-113.577872));
    std::shared_ptr<PathPoint> point4(new PathPoint(m*134.750359, m*-114.484482));
    std::shared_ptr<ControlPoint> point5(new ControlPoint(m*148.079229, m*-114.672267));
    std::shared_ptr<ControlPoint> point6(new ControlPoint(m*168.493739, m*-110.447322));
    std::shared_ptr<PathPoint> point7(new PathPoint(m*170.304549, m*-97.240702));
    std::shared_ptr<ControlPoint> point8(new ControlPoint(m*171.482419, m*-88.650189));
    std::shared_ptr<ControlPoint> point9(new ControlPoint(m*134.604629, m*-78.11541));
    std::shared_ptr<PathPoint> point10(new PathPoint(m*123.550789, m*-62.04205));
    std::shared_ptr<ControlPoint> point11(new ControlPoint(m*99.87859895, m*-27.6204));
    std::shared_ptr<ControlPoint> point12(new ControlPoint(m*116.439959, m*0));
    std::shared_ptr<PathPoint> point13(new PathPoint(m*116.439959, m*0));

    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point13));
    std::shared_ptr<Restrictor> aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    point1->setRestrictor(horizontalAxisRestrictor);
    point4->setRestrictor(aboveHorizontalRestrictor);
    point7->setRestrictor(aboveHorizontalRestrictor);
    point10->setRestrictor(aboveHorizontalRestrictor);
    point13->setRestrictor(horizontalAxisRestrictor);

    _outline->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    _outline->append(std::shared_ptr<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    _outline->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    _outline->append(std::shared_ptr<PathItem>(new CubicBezier(point10, point11, point12, point13)));
}

void Foil::initProfile()
{
    _profile = std::shared_ptr<Profile>(new Profile());
}

void Foil::initThickness()
{
    _thickness = std::shared_ptr<ThicknessProfile>(new ThicknessProfile());
}

void Foil::onFoilChanged()
{
    emit foilChanged(this);
}

void Foil::onFoilReleased()
{
    emit foilReleased(this);
}

void Foil::onProfileChanged()
{
    _thickness->setThicknessRatio(_profile->thicknessRatio());
}
