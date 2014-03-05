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

#include "path.h"
#include "pointrestrictor.h"
#include "linerestrictor.h"
#include "cubicbezier.h"

#define TTOL 0.00001

using namespace foillogic;
using namespace patheditor;

Profile::Profile(QObject *parent) :
    QObject(parent), _symmetry(Symmetry::Symmetric),
    t_topProfileTop(0.3), t_botProfileTop(0.3)
{
    initProfile();
}

std::shared_ptr<Path> Profile::topProfile()
{
    return _topProfile;
}

std::shared_ptr<Path> Profile::botProfile()
{
    return _botProfile;
}

Profile::Symmetry Profile::symmetry() const
{
    return _symmetry;
}

void Profile::setSymmetry(Symmetry symmetry)
{
    _symmetry = symmetry;

    if (_symmetry == Symmetry::Flat)
    {
        foreach (std::shared_ptr<PathItem> item, _botProfile->pathItems())
        {
            item->startPoint()->setRestrictedY(0);
            item->endPoint()->setRestrictedY(0);
            foreach (std::shared_ptr<ControlPoint> pnt, item->controlPoints())
                pnt->setRestrictedY(0);
        }
    }

    onProfileChanged(_topProfile.get());

    onProfileReleased();
}

QPointF Profile::topProfileTop(qreal *t_top) const
{
    if (t_top) *t_top = t_topProfileTop;
    return _topProfileTop;
}

QPointF Profile::bottomProfileTop(qreal *t_top) const
{
    if (t_top) *t_top = t_botProfileTop;
    return _botProfileTop;
}

qreal Profile::thickness() const
{
    return _botProfileTop.y() - _topProfileTop.y();
}

qreal Profile::thicknessRatio() const
{
    if (_topProfileTop.y() == 0)
        return 0.0000001;
    if (_botProfileTop.y() == 0)
        return 9999999;
    return -_topProfileTop.y() / _botProfileTop.y();
}

QString Profile::symmetryStr()
{
    QMetaEnum symmetryEnum = this->metaObject()->enumerator(0);
    return QString(symmetryEnum.valueToKey((int)_symmetry));
}

Path *Profile::pTopProfile()
{
    return topProfile().get();
}

Path *Profile::pBotProfile()
{
    if (symmetry() == Symmetry::Asymmetric)
        return botProfile().get();
    else
        return nullptr;
}

void Profile::setSymmetryStr(QString symmetry)
{
    QMetaEnum symmetryEnum = this->metaObject()->enumerator(0);
    bool ok;
    _symmetry = (Symmetry)symmetryEnum.keyToValue(symmetry.toStdString().c_str(), &ok);
    if (!ok)
        _symmetry = Symmetry::Symmetric;
}

void Profile::pSetTopProfile(Path *topProfile)
{
    // TODO reuse restricted points
    _topProfile = std::shared_ptr<Path>(topProfile);
}

void Profile::pSetBotProfile(Path *botProfile)
{
    // TODO reuse restricted points
    _botProfile = std::shared_ptr<Path>(botProfile);
}

void Profile::pResetBotProfile()
{
    // Do nothing, initialized in ctor
}

Profile::~Profile()
{
}

void Profile::initProfile()
{
    _topProfile = std::shared_ptr<Path>(new Path());
    _botProfile = std::shared_ptr<Path>(new Path());

    std::shared_ptr<PathPoint> point1(new PathPoint(0,0));
    std::shared_ptr<PathPoint> point3(new PathPoint(300,0));

    std::shared_ptr<PathPoint> tPoint(new PathPoint(90,-35));
    std::shared_ptr<PathPoint> bPoint(new PathPoint(90,35));

    std::shared_ptr<ControlPoint> tcPoint1(new ControlPoint(0,0));
    std::shared_ptr<ControlPoint> tcPoint2(new ControlPoint(0,-35));
    std::shared_ptr<ControlPoint> tcPoint3(new ControlPoint(135,-35));
    std::shared_ptr<ControlPoint> tcPoint4(new ControlPoint(300,0));

    std::shared_ptr<ControlPoint> bcPoint1(new ControlPoint(0,0));
    std::shared_ptr<ControlPoint> bcPoint2(new ControlPoint(0,35));
    std::shared_ptr<ControlPoint> bcPoint3(new ControlPoint(135,35));
    std::shared_ptr<ControlPoint> bcPoint4(new ControlPoint(300,0));

    std::shared_ptr<Restrictor> originRestrictor(new PointRestrictor(*point1));
    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point3));

    point1->setRestrictor(originRestrictor);
    point3->setRestrictor(horizontalAxisRestrictor);

    _tPart1 = std::shared_ptr<CubicBezier>(new CubicBezier(point1, tcPoint1, tcPoint2, tPoint));
    _tPart2 = std::shared_ptr<CubicBezier>(new CubicBezier(tPoint, tcPoint3, tcPoint4, point3));

    _bPart1 = std::shared_ptr<CubicBezier>(new CubicBezier(point1, bcPoint1, bcPoint2, bPoint));
    _bPart2 = std::shared_ptr<CubicBezier>(new CubicBezier(bPoint, bcPoint3, bcPoint4, point3));

    _topProfile->append(_tPart1);
    _topProfile->append(_tPart2);

    _botProfile->append(_bPart1);
    _botProfile->append(_bPart2);

    // connect the profiles
    connect(_topProfile.get(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(_botProfile.get(), SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(_topProfile.get(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
    connect(_botProfile.get(), SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
}

void Profile::mirror(CubicBezier *source, CubicBezier *destination)
{
    destination->startPoint()->setRestrictedPos(source->startPoint()->x(), -source->startPoint()->y());
    destination->endPoint()->setRestrictedPos(source->endPoint()->x(), -source->endPoint()->y());

    destination->controlPoint1()->setRestrictedPos(source->controlPoint1()->x(), -source->controlPoint1()->y());
    destination->controlPoint2()->setRestrictedPos(source->controlPoint2()->x(), -source->controlPoint2()->y());
}

void Profile::onProfileChanged(Path* path)
{
    if (_symmetry == Symmetry::Symmetric)
    {
        if (path == _topProfile.get())
        {
            mirror(_tPart1.get(), _bPart1.get());
            mirror(_tPart2.get(), _bPart2.get());
        }
        else
        {
            mirror(_bPart1.get(), _tPart1.get());
            mirror(_bPart2.get(), _tPart2.get());
        }
    }

    _topProfile->minY(&t_topProfileTop, TTOL);
    _botProfile->maxY(&t_botProfileTop, TTOL);
    _topProfileTop = _topProfile->pointAtPercent(t_topProfileTop);
    _botProfileTop = _botProfile->pointAtPercent(t_botProfileTop);

    emit profileChanged(this);
}

void Profile::onProfileReleased()
{
    emit profileReleased(this);
}
