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

#include "profile.h"

#include "path.h"
#include "controlpoint.h"
#include "pointrestrictor.h"
#include "linerestrictor.h"
#include "cubicbezier.h"

SERIALIZABLE(foillogic::Profile, profile)

using namespace foillogic;
using namespace patheditor;
using namespace boost::units;

Profile::Profile(QObject *parent) :
    QObject(parent), _symmetry(Symmetry::Symmetric),
    t_topProfileTop(0.3), t_botProfileTop(0.3),
    _thickness(0.01 * si::meter) // 1cm
{
    initProfile();
}

void Profile::onDeserialized()
{
    setSymmetry(_symmetry);
}

Path *Profile::topProfile()
{
    return _topProfile.get();
}

Path *Profile::botProfile()
{
    return _botProfile.get();
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

boost::units::quantity<boost::units::si::length, qreal> Profile::thickness() const
{
    return _thickness;
}

void Profile::setThickness(boost::units::quantity<boost::units::si::length, qreal> thickness)
{
    _thickness = thickness;
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

qreal Profile::pxThickness() const
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

QString Profile::symmetryStr() const
{
    QMetaEnum symmetryEnum = this->metaObject()->enumerator(0);
    return QString(symmetryEnum.valueToKey((int)_symmetry));
}

Path *Profile::pTopProfile()
{
    return topProfile();
}

Path *Profile::pBotProfile()
{
    if (symmetry() == Symmetry::Asymmetric)
        return botProfile();
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
    std::shared_ptr<Restrictor> startPntRestrictor = _topProfile->pathItems().first()->startPoint()->restrictor();
    std::shared_ptr<Restrictor> endPntRestrictor = _topProfile->pathItems().last()->endPoint()->restrictor();

    topProfile->pathItems().first()->startPoint()->setRestrictor(startPntRestrictor);
    topProfile->pathItems().last()->endPoint()->setRestrictor(endPntRestrictor);

    _botProfile->pathItems().first()->setStartPoint(topProfile->pathItems().first()->startPoint());
    _botProfile->pathItems().last()->setEndPoint(topProfile->pathItems().last()->endPoint());

    _topProfile.reset(topProfile);

    attachSignals(_topProfile.get());
}

void Profile::pSetBotProfile(Path *botProfile)
{
    std::shared_ptr<Restrictor> startPntRestrictor = _botProfile->pathItems().first()->startPoint()->restrictor();
    std::shared_ptr<Restrictor> endPntRestrictor = _botProfile->pathItems().last()->endPoint()->restrictor();

    botProfile->pathItems().first()->startPoint()->setRestrictor(startPntRestrictor);
    botProfile->pathItems().last()->endPoint()->setRestrictor(endPntRestrictor);

    _topProfile->pathItems().first()->setStartPoint(botProfile->pathItems().first()->startPoint());
    _topProfile->pathItems().last()->setEndPoint(botProfile->pathItems().last()->endPoint());

    _botProfile.reset(botProfile);

    attachSignals(_botProfile.get());
}

void Profile::pResetBotProfile()
{
    // Do nothing, initialized in ctor
}

Profile::~Profile() {}

void Profile::initProfile()
{
    _topProfile.reset(new Path());
    _botProfile.reset(new Path());

    qshared_ptr<PathPoint> point1(new PathPoint(0,0));
    qshared_ptr<PathPoint> point3(new PathPoint(300,0));

    qshared_ptr<PathPoint> tPoint(new PathPoint(90,-35));
    qshared_ptr<PathPoint> bPoint(new PathPoint(90,35));

    qshared_ptr<ControlPoint> tcPoint1(new ControlPoint(0,0));
    qshared_ptr<ControlPoint> tcPoint2(new ControlPoint(0,-35));
    qshared_ptr<ControlPoint> tcPoint3(new ControlPoint(135,-35));
    qshared_ptr<ControlPoint> tcPoint4(new ControlPoint(300,0));

    qshared_ptr<ControlPoint> bcPoint1(new ControlPoint(0,0));
    qshared_ptr<ControlPoint> bcPoint2(new ControlPoint(0,35));
    qshared_ptr<ControlPoint> bcPoint3(new ControlPoint(135,35));
    qshared_ptr<ControlPoint> bcPoint4(new ControlPoint(300,0));

    std::shared_ptr<Restrictor> originRestrictor(new PointRestrictor(*point1));
    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point3));

    point1->setRestrictor(originRestrictor);
    point3->setRestrictor(horizontalAxisRestrictor);

    _topProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point1, tcPoint1, tcPoint2, tPoint)));
    _topProfile->append(std::shared_ptr<PathItem>(new CubicBezier(tPoint, tcPoint3, tcPoint4, point3)));

    _botProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point1, bcPoint1, bcPoint2, bPoint)));
    _botProfile->append(std::shared_ptr<PathItem>(new CubicBezier(bPoint, bcPoint3, bcPoint4, point3)));

    // connect the profiles
    attachSignals(_topProfile.get());
    attachSignals(_botProfile.get());
}

void Profile::attachSignals(Path *path)
{
    connect(path, SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(path, SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));
    onProfileChanged(path);
}

void Profile::mirror(const PathItem *source, PathItem *destination)
{
    destination->startPoint()->setRestrictedPos(source->constStartPoint()->x(), -source->constStartPoint()->y());
    destination->endPoint()->setRestrictedPos(source->constEndPoint()->x(), -source->constEndPoint()->y());

    auto topContrPoints = source->constControlPoints();
    auto botContrPoints = destination->controlPoints();
    for (int i=0; i<topContrPoints.count(); i++)
    {
        botContrPoints[i]->setRestrictedPos(topContrPoints[i]->x(), -topContrPoints[i]->y());
    }
}

void Profile::onProfileChanged(Path* path)
{
    if (_symmetry == Symmetry::Symmetric)
    {
        auto topItems = _topProfile->pathItems();
        auto botItems = _botProfile->pathItems();

        if (path == _topProfile.get())
        {
            for (int i=0; i<topItems.count(); i++)
            {
                mirror(topItems[i].get(), botItems[i].get());
            }
        }
        else
        {
            for (int i=0; i<topItems.count(); i++)
            {
                mirror(botItems[i].get(), topItems[i].get());
            }
        }
    }

    _topProfile->minY(&t_topProfileTop);
    _botProfile->maxY(&t_botProfileTop);
    _topProfileTop = _topProfile->pointAtPercent(t_topProfileTop);
    _botProfileTop = _botProfile->pointAtPercent(t_botProfileTop);

    emit profileChanged(this);
}

void Profile::onProfileReleased()
{
    emit profileReleased(this);
}
