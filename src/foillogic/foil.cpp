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

#include "path.h"
#include "pathpoint.h"
#include "editablepath.h"
#include "linerestrictor.h"
#include "quadrantrestrictor.h"
#include "cubicbezier.h"
#include "outline.h"
#include "profile.h"
#include "thicknessprofile.h"
#include "pathdecorators.hpp"

SERIALIZABLE(foillogic::Foil, foil)

using namespace foillogic;
using namespace patheditor;
using namespace hrlib::patterns;

Foil::Foil(QObject *parent) :
    QObject(parent)
{
    initOutline();
    initProfile();
    initThickness();

    resetLayerCount();
}

qshared_ptr<Outline> Foil::outline()
{
    return _outline;
}

qshared_ptr<Profile> Foil::profile()
{
    return _profile;
}

qshared_ptr<ThicknessProfile> Foil::thickness()
{
    return _thickness;
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

void Foil::pSetThickness(ThicknessProfile *thickness)
{
    _thickness.reset(thickness);
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
    _thickness.reset(new ThicknessProfile());
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
    connect(_thickness.get(), SIGNAL(profileChanged(ThicknessProfile*)), this, SLOT(onFoilChanged()));
    connect(_thickness.get(), SIGNAL(profileReleased(ThicknessProfile*)), this, SLOT(onFoilReleased()));
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


std::unique_ptr<IPath> foillogic::Foil::oultineSI()
{
    // TODO scale
    return decorate<PathScaleDecorator>(_outline->path(), 1, 1);
}

std::unique_ptr<IPath> Foil::topProfileSI()
{
    // TODO scale
    return decorate<PathScaleDecorator>(_profile->topProfile(), 1, 1);
}

std::unique_ptr<IPath> Foil::botProfileSI()
{
    // TODO scale
    return decorate<PathScaleDecorator>(_profile->botProfile(), 1, 1);
}

std::unique_ptr<IPath> Foil::topThicknessSI()
{
    // TODO scale
    return decorate<PathScaleDecorator>(_thickness->topProfile(), 1, 1);
}

std::unique_ptr<IPath> Foil::botThicknessSI()
{
    // TODO scale
    return decorate<PathScaleDecorator>(_thickness->botProfile(), 1, 1);
}
