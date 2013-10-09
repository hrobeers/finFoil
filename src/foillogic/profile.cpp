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

#include "foillogic/profile.h"

#include "pointrestrictor.h"
#include "linerestrictor.h"
#include "cubicbezier.h"

using namespace foillogic;
using namespace patheditor;

Profile::Profile(QObject *parent) :
    QObject(parent)
{
    _symmetry = Symmetry::Symmetric;

    initProfile();
}

QSharedPointer<Path> Profile::topProfile()
{
    return _topProfile;
}

QSharedPointer<Path> Profile::botProfile()
{
    return _botProfile;
}

Symmetry::e Profile::symmetry() const
{
    return _symmetry;
}

void Profile::setSymmetry(Symmetry::e symmetry)
{
    _symmetry = symmetry;

    if (_symmetry == Symmetry::Flat)
    {
        foreach (QSharedPointer<PathItem> item, _botProfile->pathItems())
        {
            item->startPoint()->setRestrictedY(0);
            item->endPoint()->setRestrictedY(0);
            foreach (QSharedPointer<ControlPoint> pnt, item->controlPoints())
                pnt->setRestrictedY(0);
        }
    }

    onProfileChange(_topProfile.data());

    onProfileReleased();
}

Profile::~Profile()
{
}

void Profile::initProfile()
{
    _topProfile = QSharedPointer<Path>(new Path());
    _botProfile = QSharedPointer<Path>(new Path());

    QSharedPointer<PathPoint> point1(new PathPoint(0,0));
    QSharedPointer<PathPoint> point3(new PathPoint(200,0));

    QSharedPointer<PathPoint> tPoint(new PathPoint(60,-24));
    QSharedPointer<PathPoint> bPoint(new PathPoint(60,24));

    QSharedPointer<ControlPoint> tcPoint1(new ControlPoint(0,0));
    QSharedPointer<ControlPoint> tcPoint2(new ControlPoint(0,-24));
    QSharedPointer<ControlPoint> tcPoint3(new ControlPoint(90,-24));
    QSharedPointer<ControlPoint> tcPoint4(new ControlPoint(200,0));

    QSharedPointer<ControlPoint> bcPoint1(new ControlPoint(0,0));
    QSharedPointer<ControlPoint> bcPoint2(new ControlPoint(0,24));
    QSharedPointer<ControlPoint> bcPoint3(new ControlPoint(90,24));
    QSharedPointer<ControlPoint> bcPoint4(new ControlPoint(200,0));

    QSharedPointer<Restrictor> originRestrictor(new PointRestrictor(*point1));
    QSharedPointer<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point3));

    point1->setRestrictor(originRestrictor);
    point3->setRestrictor(horizontalAxisRestrictor);

    _tPart1 = QSharedPointer<CubicBezier>(new CubicBezier(point1, tcPoint1, tcPoint2, tPoint));
    _tPart2 = QSharedPointer<CubicBezier>(new CubicBezier(tPoint, tcPoint3, tcPoint4, point3));

    _bPart1 = QSharedPointer<CubicBezier>(new CubicBezier(point1, bcPoint1, bcPoint2, bPoint));
    _bPart2 = QSharedPointer<CubicBezier>(new CubicBezier(bPoint, bcPoint3, bcPoint4, point3));

    _topProfile->append(_tPart1);
    _topProfile->append(_tPart2);

    _botProfile->append(_bPart1);
    _botProfile->append(_bPart2);

    // connect the profiles
    connect(_topProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChange(patheditor::Path*)));
    connect(_botProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChange(patheditor::Path*)));
    connect(_topProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged()));
    connect(_botProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged()));
    connect(_topProfile.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
    connect(_botProfile.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
}

void Profile::mirror(CubicBezier *source, CubicBezier *destination)
{
    destination->startPoint()->setRestrictedPos(source->startPoint()->x(), -source->startPoint()->y());
    destination->endPoint()->setRestrictedPos(source->endPoint()->x(), -source->endPoint()->y());

    destination->controlPoint1()->setRestrictedPos(source->controlPoint1()->x(), -source->controlPoint1()->y());
    destination->controlPoint2()->setRestrictedPos(source->controlPoint2()->x(), -source->controlPoint2()->y());
}

void Profile::onProfileChange(Path* path)
{
    if (_symmetry == Symmetry::Symmetric)
    {
        if (path == _topProfile.data())
        {
            mirror(_tPart1.data(), _bPart1.data());
            mirror(_tPart2.data(), _bPart2.data());
        }
        else
        {
            mirror(_bPart1.data(), _tPart1.data());
            mirror(_bPart2.data(), _tPart2.data());
        }
    }
}

void Profile::onProfileChanged()
{
    emit profileChanged(this);
}

void Profile::onProfileReleased()
{
    emit profileReleased(this);
}
