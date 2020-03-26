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

#include "foillogic/thicknessprofile.hpp"

#include "patheditor/path.hpp"
#include "patheditor/pathpoint.hpp"
#include "patheditor/controlpoint.hpp"
#include "patheditor/curvepoint.hpp"
#include "patheditor/linerestrictor.hpp"
#include "patheditor/quadrantrestrictor.hpp"
#include "patheditor/line.hpp"
#include "patheditor/cubicbezier.hpp"

SERIALIZABLE(foillogic::ThicknessProfile, tProfile)

using namespace foillogic;
using namespace patheditor;

namespace {
  struct Flags { enum e {
                         None = 0x0,
                         Editable = 0x1,
                         Thickness = 0x0,
                         AspectRatio = 0x2,
                         Default = Thickness | Editable
    }; };
}

ThicknessProfile::ThicknessProfile(QObject *parent) :
    QObject(parent), _thicknessRatio(1),
    _flags(Flags::Default),
    _topProfile(nullptr),
    _botProfile(nullptr)
{
  init();
}

void ThicknessProfile::init()
{
  if (_topProfile) _topProfile->disconnect();

  qshared_ptr<PathPoint> point0(new CurvePoint(0,0));

  qshared_ptr<PathPoint> point1(new CurvePoint(0,-30));
  qshared_ptr<ControlPoint> point2(new ControlPoint(100,-30));
  qshared_ptr<ControlPoint> point3(new ControlPoint(300,-30));
  qshared_ptr<PathPoint> point4(new CurvePoint(300,0));

  std::unique_ptr<Path> topProfile(new Path());

  auto topProfilePath = std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4));
  topProfile->append(topProfilePath);

  // connect the profile
  pSetTopProfile(topProfile.release());
}

Path *ThicknessProfile::topProfile()
{
    return _topProfile.get();
}

Path *ThicknessProfile::botProfile()
{
    return _botProfile.get();
}

Path *ThicknessProfile::pTopProfile()
{
    return topProfile();
}

void ThicknessProfile::pSetTopProfile(Path *topProfile)
{
    auto startPoint = topProfile->pathItems().first()->startPoint();
    auto endPoint = topProfile->pathItems().last()->endPoint();

    std::shared_ptr<Restrictor> verticalAxisRestrictor(new LineRestrictor({0,0}, {0,1}));
    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor({0,0}, {1,0}));

    startPoint->setRestrictor(verticalAxisRestrictor);
    endPoint->setRestrictor(horizontalAxisRestrictor);

    std::shared_ptr<Restrictor> baseRestrictor(new QuadrantRestrictor(endPoint, Quadrants::II | Quadrants::III));
    std::shared_ptr<Restrictor> tipRestrictor(new QuadrantRestrictor(endPoint, Quadrants::II));
    if (!topProfile->pathItems().first()->controlPoints().empty())
      topProfile->pathItems().first()->controlPoints().first()->setRestrictor(baseRestrictor);
    if (!topProfile->pathItems().first()->controlPoints().empty())
      topProfile->pathItems().first()->controlPoints().last()->setRestrictor(tipRestrictor);
    if (topProfile->pathItems().size()>1)
      topProfile->pathItems().last()->startPoint()->setRestrictor(tipRestrictor);

    if (_topProfile) _topProfile->disconnect();
    _topProfile.reset(topProfile);

    // init the bot profile
    if (_botProfile) _botProfile->disconnect();
    _botProfile.reset(new Path());
    for (auto p : _topProfile->pathItems())
      _botProfile->append(p->clone());
    // Ensure the tip point is shared
    _botProfile->pathItems().last()->setEndPoint(_topProfile->pathItems().last()->endPoint());
    mirror();

    attachSignals(_topProfile.get());
}

void ThicknessProfile::resetFlags()
{
  _flags = Flags::Default;
}

bool ThicknessProfile::editable() const
{
  return _flags & Flags::Editable;
}

bool ThicknessProfile::aspectRatioEnforced() const
{
  return _flags & Flags::AspectRatio;
}

void ThicknessProfile::setAspectRatioEnforced(bool enforce)
{
  bool editable = !enforce;
  _flags = enforce ? _flags | Flags::AspectRatio : _flags ^ Flags::AspectRatio;
  _flags = editable ? _flags | Flags::Editable : _flags ^ Flags::Editable;

  if (enforce) {
    qunique_ptr<Path> topProfile(new Path());

    auto pt1 = std::make_shared<CurvePoint>(0,-30);
    auto cpt1 = std::make_shared<ControlPoint>(100,-30);
    auto cpt2 = std::make_shared<ControlPoint>(200,-30);
    auto pt2 = std::make_shared<CurvePoint>(300,-30);
    auto pt3 = std::make_shared<CurvePoint>(300,0);
    topProfile->append(std::make_shared<CubicBezier>(pt1, cpt1, cpt2, pt2));
    topProfile->append(std::make_shared<Line>(pt2, pt3));

    // connect the profile
    pSetTopProfile(topProfile.release());
  }
  else {
    init();
  }
}

ThicknessProfile::~ThicknessProfile() {}

void ThicknessProfile::setThicknessRatio(qreal profileRatio)
{
    _thicknessRatio = profileRatio;
    mirror();
}

void ThicknessProfile::attachSignals(Path *path)
{
    connect(path, SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(path, SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
}

void ThicknessProfile::mirror()
{
    PathItem* topPI = _topProfile->pathItems().first().get();
    PathItem* bottomPI = _botProfile->pathItems().first().get();

    bottomPI->startPoint()->setRestrictedPos(topPI->startPoint()->x(), -topPI->startPoint()->y()/_thicknessRatio);
    bottomPI->endPoint()->setRestrictedPos(topPI->endPoint()->x(), -topPI->endPoint()->y()/_thicknessRatio);

    auto topContrPoints = topPI->constControlPoints();
    auto botContrPoints = bottomPI->controlPoints();
    for (int i=0; i<topContrPoints.count(); i++)
    {
        botContrPoints[i]->setRestrictedPos(topContrPoints[i]->x(), -topContrPoints[i]->y()/_thicknessRatio);
    }

    emit mirrored();
}

void ThicknessProfile::onProfileChanged(Path */*unused*/)
{
    mirror();
    emit profileChanged(this);
}

void ThicknessProfile::onProfileReleased()
{
    emit profileReleased(this);
}
