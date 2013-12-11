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
    _topProfile(QSharedPointer<Path>(new Path())),
    _botProfile(QSharedPointer<Path>(new Path()))
{
    QSharedPointer<PathPoint> point0(new PathPoint(0,0));

    QSharedPointer<PathPoint> point1(new PathPoint(0,-30));
    QSharedPointer<ControlPoint> point2(new ControlPoint(0,-30));
    QSharedPointer<ControlPoint> point3(new ControlPoint(300,-30));
    QSharedPointer<PathPoint> point4(new PathPoint(300,0));

    QSharedPointer<PathPoint> point5(new PathPoint(0,30));
    QSharedPointer<ControlPoint> point6(new ControlPoint(0,30));
    QSharedPointer<ControlPoint> point7(new ControlPoint(300,30));

    QSharedPointer<Restrictor> verticalAxisRestrictor(new LineRestrictor(*point0, *point1));
    QSharedPointer<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point0, *point4));

    point1->setRestrictor(verticalAxisRestrictor);
    point4->setRestrictor(horizontalAxisRestrictor);

    _topBezier = QSharedPointer<patheditor::CubicBezier>(new CubicBezier(point1, point2, point3, point4));
    _botBezier = QSharedPointer<patheditor::CubicBezier>(new CubicBezier(point5, point6, point7, point4));

    _topProfile->append(_topBezier);
    _botProfile->append(_botBezier);

    // connect the profile
    connect(_topProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(_topProfile.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
}

QSharedPointer<Path> ThicknessProfile::topProfile()
{
    return _topProfile;
}

QSharedPointer<Path> ThicknessProfile::botProfile()
{
    return _botProfile;
}

ThicknessProfile::~ThicknessProfile()
{
}

void ThicknessProfile::setThicknessRatio(qreal profileRatio)
{
    _thicknessRatio = profileRatio;
    if (_thicknessRatio < 0.001)
        _thicknessRatio = 0.001;
    mirror();
}

void ThicknessProfile::mirror()
{
    _botBezier->startPoint()->setRestrictedPos(_topBezier->startPoint()->x(), -_topBezier->startPoint()->y()/_thicknessRatio);
    _botBezier->endPoint()->setRestrictedPos(_topBezier->endPoint()->x(), -_topBezier->endPoint()->y()/_thicknessRatio);

    _botBezier->controlPoint1()->setRestrictedPos(_topBezier->controlPoint1()->x(), -_topBezier->controlPoint1()->y()/_thicknessRatio);
    _botBezier->controlPoint2()->setRestrictedPos(_topBezier->controlPoint2()->x(), -_topBezier->controlPoint2()->y()/_thicknessRatio);

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
