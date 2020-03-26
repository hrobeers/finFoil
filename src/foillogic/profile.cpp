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

#include "foillogic/profile.hpp"

#include "patheditor/path.hpp"
#include "patheditor/controlpoint.hpp"
#include "patheditor/curvepoint.hpp"
#include "patheditor/pointrestrictor.hpp"
#include "patheditor/linerestrictor.hpp"
#include "patheditor/quadrantrestrictor.hpp"
#include "patheditor/pathrestrictor.hpp"
#include "patheditor/cubicbezier.hpp"

SERIALIZABLE(foillogic::Profile, profile)

using namespace foillogic;
using namespace patheditor;
using namespace boost::units;

namespace {
  struct Flags { enum e {
    None = 0x0,
    Editable = 0x1,
    Default = Editable
  }; };
}

Profile::Profile(QObject *parent) :
    QObject(parent), _symmetry(Symmetry::Symmetric),
    t_topProfileTop(0.3), t_botProfileTop(0.3), _flags(Flags::Default)
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

    if (_symmetry != Symmetry::Symmetric)
    {
        foreach (std::shared_ptr<PathItem> item, _botProfile->pathItems())
        {
            item->startPoint()->setContinuous(false);
            item->endPoint()->setContinuous(false);
        }
    }

    onProfileChanged(_topProfile.get());
    onProfileReleased();
    emit symmetryChanged(_symmetry);
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

bool Profile::editable() const
{
  return _flags & Flags::Editable;
}

void Profile::setEditable(bool editable)
{
  _flags = editable ? _flags | Flags::Editable : _flags ^ Flags::Editable;
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

int Profile::flags() const
{
    return _flags;
}

void Profile::setSymmetryStr(QString symmetry)
{
    QMetaEnum symmetryEnum = this->metaObject()->enumerator(0);
    bool ok;
    _symmetry = (Symmetry)symmetryEnum.keyToValue(symmetry.toUtf8().constData(), &ok);
    if (!ok)
        _symmetry = Symmetry::Symmetric;
}

void Profile::pSetTopProfile(Path *topProfile)
{
    if (_topProfile) _topProfile->disconnect();
    _topProfile.reset(topProfile);

    const std::shared_ptr<Restrictor> originRestrictor(new PointRestrictor({0,0}));
    const std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor({0,0},{1,0}));
    const std::shared_ptr<Restrictor> QI(new QuadrantRestrictor(Quadrants::I));

    for (auto pi : topProfile->pathItems()) {
      pi->startPoint()->setRestrictor(QI);
      pi->endPoint()->setRestrictor(QI);
    }
    // Overwrite start & end restrictors
    topProfile->pathItems().first()->startPoint()->setRestrictor(originRestrictor);
    topProfile->pathItems().last()->endPoint()->setRestrictor(horizontalAxisRestrictor);
    // Set the first control point restrictor if it exists
    if (topProfile->pathItems().first()->controlPoints().size())
      topProfile->pathItems().first()->controlPoints().first()->setRestrictor(QI);
    // Set the last control point restrictor if it exists
    if (topProfile->pathItems().last()->controlPoints().size())
      topProfile->pathItems().last()->controlPoints().last()
        ->setRestrictor(std::shared_ptr<Restrictor>(new QuadrantRestrictor(topProfile->pathItems().last()->endPoint(), Quadrants::II)));

    if (_botProfile) {
      _botProfile->pathItems().first()->setStartPoint(topProfile->pathItems().first()->startPoint());
      _botProfile->pathItems().last()->setEndPoint(topProfile->pathItems().last()->endPoint());
    }

    attachSignals(_topProfile.get());
}

void Profile::pSetBotProfile(Path *botProfile)
{
    if (_botProfile) _botProfile->disconnect();
    _botProfile.reset(botProfile);

    const std::shared_ptr<Restrictor> originRestrictor(new PointRestrictor({0,0}));
    const std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor({0,0},{1,0}));
    const std::shared_ptr<Restrictor> QIV(new QuadrantRestrictor(Quadrants::IV));

    // Set the end and start restrictors
    botProfile->pathItems().first()->startPoint()->setRestrictor(originRestrictor);
    botProfile->pathItems().last()->endPoint()->setRestrictor(horizontalAxisRestrictor);
    // Set the first control point restrictor if it exists
    if (botProfile->pathItems().first()->controlPoints().size())
      botProfile->pathItems().first()->controlPoints().first()->setRestrictor(QIV);
    // Set the last control point restrictor if it exists
    if (botProfile->pathItems().last()->controlPoints().size())
      botProfile->pathItems().last()->controlPoints().last()
        ->setRestrictor(std::shared_ptr<Restrictor>(new QuadrantRestrictor(botProfile->pathItems().last()->endPoint(), Quadrants::III)));

    attachSignals(_botProfile.get());
}

void Profile::setFlags(int flags)
{
  _flags = flags;
}

void Profile::pResetBotProfile()
{
  // Do nothing, initialized in ctor
}

void Profile::resetFlags()
{
  _flags = Flags::Default;
}

Profile::~Profile() {}

void Profile::initProfile()
{
    if (_topProfile) _topProfile->disconnect();
    if (_botProfile) _botProfile->disconnect();

    std::unique_ptr<Path> topProfile(new Path());
    std::unique_ptr<Path> botProfile(new Path());

    qshared_ptr<PathPoint> point1(new CurvePoint(0,0));
    qshared_ptr<PathPoint> point3(new CurvePoint(300,0));

    qshared_ptr<PathPoint> tPoint(new CurvePoint(90,-35));
    qshared_ptr<PathPoint> bPoint(new CurvePoint(90,35));

    qshared_ptr<ControlPoint> tcPoint1(new ControlPoint(0,0));
    qshared_ptr<ControlPoint> tcPoint2(new ControlPoint(0,-35));
    qshared_ptr<ControlPoint> tcPoint3(new ControlPoint(135,-35));
    qshared_ptr<ControlPoint> tcPoint4(new ControlPoint(280,-7));

    qshared_ptr<ControlPoint> bcPoint1(new ControlPoint(0,0));
    qshared_ptr<ControlPoint> bcPoint2(new ControlPoint(0,35));
    qshared_ptr<ControlPoint> bcPoint3(new ControlPoint(135,35));
    qshared_ptr<ControlPoint> bcPoint4(new ControlPoint(280,7));

    topProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point1, tcPoint1, tcPoint2, tPoint)));
    topProfile->append(std::shared_ptr<PathItem>(new CubicBezier(tPoint, tcPoint3, tcPoint4, point3)));

    botProfile->append(std::shared_ptr<PathItem>(new CubicBezier(point1, bcPoint1, bcPoint2, bPoint)));
    botProfile->append(std::shared_ptr<PathItem>(new CubicBezier(bPoint, bcPoint3, bcPoint4, point3)));


    // connect the profiles
    pSetTopProfile(topProfile.release());
    pSetBotProfile(botProfile.release());

    tPoint->setContinuous(true);
    bPoint->setContinuous(true);

    onProfileChanged(_topProfile.get());
    onProfileChanged(_botProfile.get());
}

void Profile::attachSignals(Path *path)
{
    connect(path, SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onProfileChanged(patheditor::Path*)));
    connect(path, SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onProfileReleased()));

    // Add bottom profile restrictions
    if (_topProfile && _botProfile) {
      _topProfile->pathItems().first()->setStartPoint(_botProfile->pathItems().first()->startPoint());
      _topProfile->pathItems().last()->setEndPoint(_botProfile->pathItems().last()->endPoint());

      std::shared_ptr<Restrictor> botThickRestrictor(new PathRestrictor(_topProfile, [](qreal */*x*/, qreal *y, Path* path)
                                                                            {
                                                                              qreal minY=0;
                                                                              for (auto pi : path->pathItems())
                                                                                for (auto p : pi->points())
                                                                                  if (p.y()<minY)
                                                                                    minY = p.y();
                                                                              *y = std::min(-minY, *y);
                                                                            }));
      for (auto pi : _botProfile->pathItems()) {
        if (pi != _botProfile->pathItems().last())
          pi->endPoint()->setRestrictor(botThickRestrictor);
      }
    }
}

void Profile::mirror(const PathItem *source, PathItem *destination)
{
    destination->startPoint()->setPos(source->constStartPoint()->x(), -source->constStartPoint()->y());
    destination->endPoint()->setPos(source->constEndPoint()->x(), -source->constEndPoint()->y());

    auto topContrPoints = source->constControlPoints();
    auto botContrPoints = destination->controlPoints();
    for (int i=0; i<topContrPoints.count(); i++)
    {
        botContrPoints[i]->setPos(topContrPoints[i]->x(), -topContrPoints[i]->y());
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
