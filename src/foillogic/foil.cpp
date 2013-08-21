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
#include "quadrantrestrictor.h"
#include "linerestrictor.h"
#include "pointrestrictor.h"
#include "cubicbezier.h"

using namespace foillogic;
using namespace patheditor;

Foil::Foil(QObject *parent) :
    QObject(parent)
{
    _symmetric = true;

    initOutline();
    initProfile();
    initThickness();

    connect(_outline.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));
    connect(_topProfile.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));
    connect(_thickness.data(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onFoilChanged()));

    connect(_outline.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
    connect(_topProfile.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
    connect(_thickness.data(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onFoilReleased()));
}

QSharedPointer<Path> Foil::outline()
{
    return _outline;
}

QSharedPointer<Path> Foil::topProfile()
{
    return _topProfile;
}

QSharedPointer<Path> Foil::botProfile()
{
    return _botProfile;
}

QSharedPointer<Path> Foil::thickness()
{
    return _thickness;
}

bool Foil::symmetric()
{
    return _symmetric;
}

void Foil::setSymmetric(bool symmetric)
{
    _symmetric = symmetric;
    onProfileChange(_topProfile.data());
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

void Foil::mirror(CubicBezier *source, CubicBezier *destination)
{
    destination->startPoint()->setRestrictedPos(source->startPoint()->x(), -source->startPoint()->y());
    destination->endPoint()->setRestrictedPos(source->endPoint()->x(), -source->endPoint()->y());

    destination->controlPoint1()->setRestrictedPos(source->controlPoint1()->x(), -source->controlPoint1()->y());
    destination->controlPoint2()->setRestrictedPos(source->controlPoint2()->x(), -source->controlPoint2()->y());
}

void Foil::onFoilChanged()
{
    emit foilChanged(this);
}

void Foil::onFoilReleased()
{
    emit foilReleased(this);
}

void Foil::onProfileChange(Path* path)
{
    if (_symmetric)
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
