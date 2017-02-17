
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

#include "patheditor/path.hpp"
#include "patheditor/pathpoint.hpp"
#include "patheditor/editablepath.hpp"
#include "patheditor/linerestrictor.hpp"
#include "patheditor/quadrantrestrictor.hpp"
#include "patheditor/cubicbezier.hpp"
#include "outline.h"
#include "profile.h"
#include "thicknessprofile.h"
#include "patheditor/pathdecorators.hpp"

SERIALIZABLE(foillogic::Foil, foil)

using namespace foillogic;
using namespace patheditor;
using namespace hrlib::patterns;
using namespace boost::units;

Foil::Foil(QObject *parent) :
    QObject(parent),
    _thickness(0.01 * si::meter) // 1cm
{
    initOutline();
    initProfile();
    initThickness();

    resetLayerCount();
    pSetMinThickness(0.0008);
}

void Foil::onDeserialized()
{
    _thicknessProfile->setThicknessRatio(_profile->thicknessRatio());
}

void Foil::pSetOutline(Outline *outline)
{
    _outline.reset(outline);
    connectOutline();
}

void Foil::pSetProfile(Profile *profile)
{
    _profile.reset(profile);
    connectProfile();
}

void Foil::pSetThicknessProfile(ThicknessProfile *thickness)
{
    _thicknessProfile.reset(thickness);
    connectThickness();
}

void Foil::setLayerCount(int layerCount)
{
    _layerCount = layerCount;
}

void Foil::resetLayerCount()
{
    setLayerCount(6);
}

void Foil::pResetThickness()
{
    pSetThickness(_profile->_thickness_legacy);
}

void Foil::pResetMinThickness()
{
    _minThickness = 0;
}

foillogic::Foil::~Foil() { }

void Foil::initOutline()
{
    _outline.reset(new Outline());
    connectOutline();
}

void Foil::initProfile()
{
    _profile.reset(new Profile());
    connectProfile();
}

void Foil::initThickness()
{
    _thicknessProfile.reset(new ThicknessProfile());
    connectThickness();
}

void Foil::connectOutline()
{
    connect(_outline.get(), SIGNAL(outlineChanged(Outline*)), this, SLOT(onFoilChanged()));
    connect(_outline.get(), SIGNAL(outlineReleased(Outline*)), this, SLOT(onFoilReleased()));
}

void Foil::connectProfile()
{
    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onFoilChanged()));
    connect(_profile.get(), SIGNAL(profileReleased(Profile*)), this, SLOT(onFoilReleased()));

    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onProfileChanged()));
}

void Foil::connectThickness()
{
    connect(_thicknessProfile.get(), SIGNAL(profileChanged(ThicknessProfile*)), this, SLOT(onFoilChanged()));
    connect(_thicknessProfile.get(), SIGNAL(profileReleased(ThicknessProfile*)), this, SLOT(onFoilReleased()));
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
    if (_thicknessProfile)
        _thicknessProfile->setThicknessRatio(_profile->thicknessRatio());
}


std::unique_ptr<IPath> foillogic::Foil::outlineSI()
{
    qreal t_top = 0.3;
    quantity<si::length, qreal> height = _outline->height();
    qreal s = height.value() / _outline->path()->minY(&t_top);
    return decorate<PathScaleDecorator>(_outline->path(), s, s);
}

namespace {
    qreal scaleFactorX(IPath *basePath, qreal xSI)
    {
        qreal baseLength = basePath->pointAtPercent(1).x() - basePath->pointAtPercent(0).x();
        return xSI / baseLength;
    }

    std::pair<qreal, qreal> profileScaleFactors(Foil* self)
    {
        std::pair<qreal, qreal> retVal;

        // determine the x scaling factor
        auto pOutlineSI = self->outlineSI();
        qreal baseLengthSI = pOutlineSI->pointAtPercent(1).x() - pOutlineSI->pointAtPercent(0).x();
        retVal.first = qAbs(scaleFactorX(self->profile()->topProfile(), baseLengthSI));

        // determine the y scaling factor
        quantity<si::length, qreal> thicknessSI = self->thickness();
        qreal t_top = 0.3;
        retVal.second = qAbs(thicknessSI.value() / (self->profile()->topProfileTop(&t_top).y() - self->profile()->bottomProfileTop(&t_top).y()));

        return retVal;
    }

    std::pair<qreal, qreal> thicknessScaleFactors(Foil* self)
    {
        std::pair<qreal, qreal> retVal;

        // determine the x scaling factor
        quantity<si::length, qreal> heightSI = self->outline()->height();
        retVal.first = qAbs(scaleFactorX(self->thicknessProfile()->topProfile(), heightSI.value()));

        // determine the y scaling factor
        quantity<si::length, qreal> thicknessSI = self->thickness();
        qreal t_top = 0;
        retVal.second = qAbs(thicknessSI.value() / (self->thicknessProfile()->topProfile()->minY(&t_top) - self->thicknessProfile()->botProfile()->maxY(&t_top)));

        return retVal;
    }
}

std::unique_ptr<IPath> Foil::topProfileSI()
{
    auto s = profileScaleFactors(this);
    return decorate<PathScaleDecorator>(_profile->topProfile(), s.first, s.second);
}

std::unique_ptr<IPath> Foil::botProfileSI()
{
    auto s = profileScaleFactors(this);
    return decorate<PathScaleDecorator>(_profile->botProfile(), s.first, s.second);
}

std::unique_ptr<IPath> Foil::topThicknessSI()
{
    auto s = thicknessScaleFactors(this);
    return decorate<PathScaleDecorator>(_thicknessProfile->topProfile(), s.first, s.second);
}

std::unique_ptr<IPath> Foil::botThicknessSI()
{
    auto s = thicknessScaleFactors(this);
    return decorate<PathScaleDecorator>(_thicknessProfile->botProfile(), s.first, s.second);
}

boost::units::quantity<boost::units::si::length, qreal> Foil::thickness() const
{
    return _thickness;
}

void Foil::setThickness(boost::units::quantity<boost::units::si::length, qreal> thickness)
{
    _thickness = thickness;
}

boost::units::quantity<si::length, qreal> Foil::minThickness() const
{
    return _minThickness;
}

void Foil::setMinThickness(boost::units::quantity<si::length, qreal> minThickness)
{
    _minThickness = minThickness;
}
