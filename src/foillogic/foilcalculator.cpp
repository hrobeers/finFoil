/****************************************************************************

 Copyright (c) 2013, Hans Robeers
 All rights reserved.

 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

****************************************************************************/

#include "foillogic/foilcalculator.hpp"

#include <QtMath>
#include "patheditor/path.hpp"
#include "foillogic/foil.hpp"
#include "foillogic/profile.hpp"
#include "foillogic/outline.hpp"
#include "foillogic/contourcalculator.hpp"

using namespace foillogic;
using namespace boost::units;

FoilCalculator::FoilCalculator(Foil *foil) :
    QObject(), _calculated(false)
{
    _foil = foil;

    setEquidistantContours(_foil->layerCount());

    connect(_foil, SIGNAL(foilChanged(Foil*)), this, SLOT(foilChanged()));
    connect(_foil, SIGNAL(foilReleased(Foil*)), this, SLOT(foilReleased()));
}

Foil *FoilCalculator::foil()
{
    return _foil;
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

    qSort(thicknesses);
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

void FoilCalculator::calculate(bool fastCalc)
{
    _topContours.clear();
    _botContours.clear();
#ifdef SERIAL
    foreach (qreal thickness, _contourThicknesses)
    {
        if (inProfileSide(thickness, Side::Top))
        {
            qreal specificPerc = -_foil->profile()->thickness() * (thickness - 1)/_foil->profile()->topProfileTop().y() + 1;
            std::shared_ptr<QPainterPath> topPath(new QPainterPath());
            _contours.append(topPath);
            ContourCalculator<QPainterPath> tcCalc(specificPerc, _foil, topPath.get(), Side::Top, fastCalc);
            tcCalc.run();
        }

        if (inProfileSide(thickness, Side::Bottom))
        {
            qreal specificPerc = -(_foil->profile()->thickness() * (thickness - 1)/_foil->profile()->bottomProfileTop().y() + thicknessRatio);
            std::shared_ptr<QPainterPath> botPath(new QPainterPath());
            _botContours.append(botPath);
            ContourCalculator<QPainterPath> bcCalc(specificPerc, _foil, botPath.get(), Side::Bottom, fastCalc);
            bcCalc.run();
        }

        AreaCalculator aCalc(_foil);
        SweepCalculator sCalc(_foil);

        aCalc.run();
        sCalc.run();
    }
#endif
#ifndef SERIAL
    qreal thicknessRatio = _foil->profile()->thicknessRatio();
    foreach (qreal thickness, _contourThicknesses)
    {
        if (inProfileSide(thickness, Side::Top))
        {
            qreal specificPerc = -_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->topProfileTop().y() + 1;
            std::shared_ptr<QPainterPath> topPath(new QPainterPath());
            _topContours.append(topPath);
            _tPool.start(new ContourCalculator<QPainterPath>(specificPerc, _foil, topPath.get(), Side::Top, fastCalc));
        }

        if (inProfileSide(thickness, Side::Bottom))
        {
            qreal specificPerc = -(_foil->profile()->pxThickness() * (thickness - 1)/_foil->profile()->bottomProfileTop().y() + thicknessRatio);
            std::shared_ptr<QPainterPath> botPath(new QPainterPath());
            _botContours.prepend(botPath);
            _tPool.start(new ContourCalculator<QPainterPath>(specificPerc, _foil, botPath.get(), Side::Bottom, fastCalc));
        }
    }

    _tPool.start(new AreaCalculator(_foil));
    _tPool.start(new SweepCalculator(_foil));

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
    AreaCalculator aCalc(_foil);
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


AreaCalculator::AreaCalculator(Foil *foil)
{
    _foil = foil;
}

void AreaCalculator::run()
{
    qreal outlineTop = _foil->outline()->path()->minY();
    qreal scalefactor = qPow(_foil->outline()->height().value() / qAbs(outlineTop), 2);
    qreal smArea = _foil->outline()->path()->area() * scalefactor;
    quantity<si::area, qreal> area = quantity<si::area, qreal>(smArea * si::square_meter);
    _foil->outline()->setArea(area);
}


SweepCalculator::SweepCalculator(Foil *foil)
{
    _foil = foil;
}

void SweepCalculator::run()
{
    // find top and outline edges
    qreal t_top = 0;
    _foil->outline()->path()->minY(&t_top);
    QPointF top = _foil->outline()->path()->pointAtPercent(t_top);
    qreal oLEdge = _foil->outline()->path()->pointAtPercent(0).x();
    qreal oTEdge = _foil->outline()->path()->pointAtPercent(1).x();

    // find thickest point
    qreal t_thick = 0;
    _foil->profile()->topProfile()->minY(&t_thick);
    qreal thick = _foil->profile()->topProfile()->pointAtPercent(t_thick).x();
    qreal pEdge = _foil->profile()->topProfile()->pointAtPercent(1).x();
    qreal thickX = thick/pEdge * (oTEdge - oLEdge) + oLEdge;

    // calculate the sweep angle in degrees
    qreal os = top.x() - thickX;
    qreal ns = -top.y();
    quantity<si::plane_angle, qreal> sweep(qAtan(os/ns) * si::radian);
    _foil->outline()->setSweep(sweep);
}
