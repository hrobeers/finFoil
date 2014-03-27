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

#include "thicknessprofile.h"

#include "path.h"
#include "pathpoint.h"
#include "controlpoint.h"
#include "linerestrictor.h"
#include "cubicbezier.h"

using namespace foillogic;
using namespace patheditor;

ThicknessProfile::ThicknessProfile(QObject *parent) :
    QObject(parent), _thicknessRatio(1),
    _topProfile(std::shared_ptr<Path>(new Path())),
    _botProfile(std::shared_ptr<Path>(new Path()))
{
    std::shared_ptr<PathPoint> point0(new PathPoint(0,0));

    std::shared_ptr<PathPoint> point1(new PathPoint(0,-30));
    std::shared_ptr<ControlPoint> point2(new ControlPoint(0,-30));
    std::shared_ptr<ControlPoint> point3(new ControlPoint(300,-30));
    std::shared_ptr<PathPoint> point4(new PathPoint(300,0));

    std::shared_ptr<PathPoint> point5(new PathPoint(0,30));
    std::shared_ptr<ControlPoint> point6(new ControlPoint(0,30));
    std::shared_ptr<ControlPoint> point7(new ControlPoint(300,30));

    std::shared_ptr<Restrictor> verticalAxisRestrictor(new LineRestrictor(*point0, *point1));
    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point0, *point4));

    point1->setRestrictor(verticalAxisRestrictor);
    point4->setRestrictor(horizontalAxisRestrictor);

    _topProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    _botProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point5, point6, point7, point4)));

    // connect the profile
    attachSignals(_topProfile.get());
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
    std::shared_ptr<Restrictor> startPntRestrictor = _topProfile->pathItems().first()->startPoint()->restrictor();
    std::shared_ptr<Restrictor> endPntRestrictor = _topProfile->pathItems().last()->endPoint()->restrictor();

    topProfile->pathItems().first()->startPoint()->setRestrictor(startPntRestrictor);
    topProfile->pathItems().last()->endPoint()->setRestrictor(endPntRestrictor);

    _topProfile.reset(topProfile);

    attachSignals(_topProfile.get());
}

ThicknessProfile::~ThicknessProfile()
{
}

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
