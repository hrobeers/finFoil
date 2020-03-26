/****************************************************************************

 Copyright (c) 2020, Hans Robeers
 All rights reserved.

 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

****************************************************************************/

#include "foillogic/foilcalculator.hpp"

#include <QtMath>
#include "patheditor/path.hpp"
#include "foillogic/contourcalculator.hpp"
#include "foillogic/foil.hpp"
#include "foillogic/profile.hpp"
#include "foillogic/outline.hpp"
#include "foillogic/thicknessprofile.hpp"
#include "hrlib/patterns/decorator.hpp"
#include "patheditor/pathdecorators.hpp"

using namespace foillogic;
using namespace boost::units;
using namespace hrlib::patterns;

#ifdef QT_DEBUG
    const size_t LOW_SEC = 16;
    const size_t HI_SEC = 64;
    const size_t LOW_RES = 20;
    const size_t HI_RES = 50;
#else
    const size_t LOW_SEC = 128;
    const size_t HI_SEC = 512;
    const size_t LOW_RES = 200;
    const size_t HI_RES = 500;
#endif

FoilCalculator::FoilCalculator(Foil *foil) :
    QObject(), _calculated(false)
{
  setFoil(foil);
}

Foil *FoilCalculator::foil()
{
  return _foil;
}

void FoilCalculator::setFoil(Foil *foil)
{
    _foil = foil;

    setEquidistantContours(_foil->layerCount());

    connect(_foil, SIGNAL(foilChanged(Foil*)), this, SLOT(foilChanged()), Qt::UniqueConnection);
    connect(_foil, SIGNAL(foilReleased(Foil*)), this, SLOT(foilReleased()), Qt::UniqueConnection);
}

QList<qreal> FoilCalculator::contourThicknesses() const
{
    return _contourThicknesses;
}

void FoilCalculator::setContourThicknesses(QList<qreal> thicknesses)
{
    _contourThicknesses = thicknesses;
    calculate(false);
}

void FoilCalculator::setEquidistantContours(int contourCount)
{
    _foil->setLayerCount(contourCount);
    qreal increment = qreal(1) / qreal(contourCount);
    qreal thickness = 0;
    QList<qreal> thicknesses;
    for (int i = 0; i < contourCount - 1; i++)
    {
        thickness += increment;
        thicknesses.append(thickness);
    }

    std::sort(thicknesses.begin(), thicknesses.end());
    setContourThicknesses(thicknesses);
}

QList<std::shared_ptr<QPainterPath> > FoilCalculator::topContours()
{
    return _topContours;
}

QList<std::shared_ptr<QPainterPath> > FoilCalculator::bottomContours()
{
    return _botContours;
}

struct invQPainterPath
{
  QPainterPath* _p;
  explicit invQPainterPath(QPainterPath *p) : _p(p) {}

  inline void lineTo(qreal x, qreal y) { _p->lineTo(x, -y); }
  inline void lineTo(const QPointF &p) { lineTo(p.x(), p.y()); }

  inline void moveTo(qreal x, qreal y) { _p->moveTo(x, -y); }
  inline void moveTo(const QPointF &p) { moveTo(p.x(), p.y()); }
};

void FoilCalculator::calculate(bool fastCalc)
{
    _topContours.clear();
    _botContours.clear();

    size_t sectionCount = fastCalc? LOW_SEC : HI_SEC;
    size_t resolution = fastCalc? LOW_RES : HI_RES;

    auto outline = decorate<PathScaleDecorator>(_foil->outline()->path(),1,-1);
    auto topThickness = decorate<PathScaleDecorator>(_foil->thicknessProfile()->topProfile(),1,-1);
    auto topProfile = decorate<PathScaleDecorator>(_foil->profile()->topProfile(),1,-1);

    qreal thicknessRatio = _foil->profile()->thicknessRatio();
#ifdef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        if (inProfileSide(thickness, Side::Top))
        {
            qreal specificPerc = -_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->topProfileTop().y() + 1;
            std::shared_ptr<QPainterPath> topPath(new QPainterPath());
            _topContours.append(topPath);
            std::unique_ptr<invQPainterPath> path(new invQPainterPath(topPath.get()));
            ContourCalculator<invQPainterPath> tcCalc(path.get(), specificPerc,
                                                      outline.get(),
                                                      topThickness.get(),
                                                      topProfile.get(),
                                                      _foil->thicknessProfile()->aspectRatioEnforced(),
                                                      sectionCount, resolution);
            tcCalc.run();
        }

        if (inProfileSide(thickness, Side::Bottom))
        {
            qreal specificPerc = -(_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->bottomProfileTop().y() + thicknessRatio);
            std::shared_ptr<QPainterPath> botPath(new QPainterPath());
            _botContours.push_front(botPath);
            std::unique_ptr<invQPainterPath> path(new invQPainterPath(botPath.get()));
            ContourCalculator<invQPainterPath> bcCalc(path.get(), specificPerc,
                                                      outline.get(),
                                                      _foil->thicknessProfile()->botProfile(),
                                                      _foil->profile()->botProfile(),
                                                      _foil->thicknessProfile()->aspectRatioEnforced(),
                                                      sectionCount, resolution);
            bcCalc.run();
        }

        AreaSweepCalculator aCalc(_foil);

        aCalc.run();
    }
#endif
#ifndef SERIAL
    std::list<std::unique_ptr<invQPainterPath>> painterscope;
    foreach (qreal thickness, _contourThicknesses)
    {
        if (inProfileSide(thickness, Side::Top))
        {
            qreal specificPerc = -_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->topProfileTop().y() + 1;
            std::shared_ptr<QPainterPath> topPath(new QPainterPath());
            _topContours.append(topPath);
            std::unique_ptr<invQPainterPath> path(new invQPainterPath(topPath.get()));
            _tPool.start(new ContourCalculator<invQPainterPath>(path.get(), specificPerc,
                                                                outline.get(),
                                                                topThickness.get(),
                                                                topProfile.get(),
                                                                _foil->thicknessProfile()->aspectRatioEnforced(),
                                                                sectionCount, resolution));
            painterscope.push_back(std::move(path));
        }

        if (inProfileSide(thickness, Side::Bottom))
        {
            qreal specificPerc = -(_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->bottomProfileTop().y() + thicknessRatio);
            std::shared_ptr<QPainterPath> botPath(new QPainterPath());
            _botContours.push_front(botPath);
            std::unique_ptr<invQPainterPath> path(new invQPainterPath(botPath.get()));
            _tPool.start(new ContourCalculator<invQPainterPath>(path.get(), specificPerc,
                                                                outline.get(),
                                                                _foil->thicknessProfile()->botProfile(),
                                                                _foil->profile()->botProfile(),
                                                                _foil->thicknessProfile()->aspectRatioEnforced(),
                                                                sectionCount, resolution));
            painterscope.push_back(std::move(path));
        }
    }

    _tPool.start(new AreaSweepCalculator(_foil));

    _tPool.waitForDone();
#endif

    _calculated = true;
    emit foilCalculated(this);
}

bool FoilCalculator::calculated() const
{
    return _calculated;
}

void FoilCalculator::recalculateArea()
{
    AreaSweepCalculator aCalc(_foil);
    aCalc.run();
}

bool FoilCalculator::inProfileSide(qreal thicknessPercent, Side::e side)
{
    Profile* profile = _foil->profile();

    switch (side) {
    case Side::Bottom:
        if (thicknessPercent - (profile->bottomProfileTop().y()-profile->botProfile()->minY())/profile->pxThickness() < 0.1)
            return true;
        return false;
    default:
        if ((profile->bottomProfileTop().y()-profile->topProfile()->maxY())/profile->pxThickness() - thicknessPercent < 0.1)
            return true;
        return false;
    }
}

void FoilCalculator::foilChanged()
{
    calculate(true);
}

void FoilCalculator::foilReleased()
{
    calculate(false);
}


AreaSweepCalculator::AreaSweepCalculator(Foil *foil)
{
    _foil = foil;
}


#include <boost/geometry.hpp>
typedef boost::geometry::model::ring<QPointF> ring;
#include <boost/geometry/geometries/register/point.hpp>
BOOST_GEOMETRY_REGISTER_POINT_2D_GET_SET(QPointF, qreal, cs::cartesian, x, y, setX, setY)

void AreaSweepCalculator::run()
{
    Path* outlinePath = _foil->outline()->path();
    qreal outlineTop = outlinePath->minY();
    qreal scalefactor = qPow(_foil->outline()->height().value() / qAbs(outlineTop), 2);

    const int resolution = 512;
    qreal percStep = 1 / qreal(resolution-1);
    ring points;
    qreal perc = 0;
    for (int i = 0; i < resolution; i++)
      {
        QPointF pnt = outlinePath->pointAtPercent(perc);
        points.push_back(pnt);
        perc += percStep;
      }

    // calculate the area
    qreal smArea = std::abs(boost::geometry::area(points)) * scalefactor;
    quantity<si::area, qreal> area = quantity<si::area, qreal>(smArea * si::square_meter);
    _foil->outline()->setArea(area);

    // calculate the sweep angle
    QPointF centroid;
    boost::geometry::centroid(points, centroid);
    qreal xHalfBase = _foil->outline()->path()->pointAtPercent(1).x()/2;
    qreal os = centroid.x() - xHalfBase;
    qreal ns = -centroid.y();
    quantity<si::plane_angle, qreal> sweep(qAtan(os/ns) * si::radian);
    _foil->outline()->setSweep(sweep);

    // recalc thickness if AR enforced
    if (_foil->aspectRatioEnforced()) {
      // TODO recalc on depth change
      auto topProfile = _foil->profile()->topProfile();
      auto botProfile = _foil->profile()->botProfile();
      auto aspectRatio = (botProfile->maxY() - topProfile->minY()) /
        (topProfile->pointAtPercent(1).x() - topProfile->pointAtPercent(0).x());
      auto thickness = (_foil->outlineSI()->pointAtPercent(1).x() - _foil->outlineSI()->pointAtPercent(0).x())*aspectRatio;
      _foil->pSetThickness(thickness);
    }
}
