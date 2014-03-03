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
#include "outline.h"
#include "profile.h"
#include "thicknessprofile.h"

using namespace foillogic;
using namespace patheditor;

Foil::Foil(QObject *parent) :
    QObject(parent)
{
    initOutline();
    initProfile();
    initThickness();

    connect(_outline.get(), SIGNAL(outlineChanged(Outline*)), this, SLOT(onFoilChanged()));
    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onFoilChanged()));
    connect(_thickness.get(), SIGNAL(profileChanged(ThicknessProfile*)), this, SLOT(onFoilChanged()));

    connect(_profile.get(), SIGNAL(profileChanged(Profile*)), this, SLOT(onProfileChanged()));

    connect(_outline.get(), SIGNAL(outlineReleased(Outline*)), this, SLOT(onFoilReleased()));
    connect(_profile.get(), SIGNAL(profileReleased(Profile*)), this, SLOT(onFoilReleased()));
    connect(_thickness.get(), SIGNAL(profileReleased(ThicknessProfile*)), this, SLOT(onFoilReleased()));
}

std::shared_ptr<Outline> Foil::outline()
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

Foil::~Foil()
{
}

void Foil::initOutline()
{
    _outline = std::shared_ptr<Outline>(new Outline());
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
