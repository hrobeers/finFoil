/****************************************************************************

 Copyright (c) 2014, Hans Robeers
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

#include "outline.h"

#include "path.h"
#include "cubicbezier.h"
#include "linerestrictor.h"
#include "quadrantrestrictor.h"

using namespace foillogic;
using namespace patheditor;
using namespace boost::units;

Outline::Outline(QObject *parent) :
    QObject(parent)
{
    initPath();

    _height = quantity<si::length, qreal>(0.1 * si::meter); // 10cm
    _area = quantity<si::area, qreal>(0 * si::meter * si::meter);
}

Path *Outline::path()
{
    return _path.get();
}

quantity<si::length, qreal> Outline::height()
{
    return _height;
}

quantity<si::area, qreal> Outline::area()
{
    return _area;
}

boost::units::quantity<si::plane_angle, qreal> Outline::sweep()
{
    return _sweep;
}

void Outline::setHeight(quantity<si::length, qreal> height)
{
    _height = height;
}

void Outline::setArea(quantity<si::area, qreal> area)
{
    _area = area;
}

void Outline::setSweep(boost::units::quantity<si::plane_angle, qreal> sweep)
{
    _sweep = sweep;
}

Path *Outline::pPath()
{
    return path();
}

void Outline::pSetPath(Path *path)
{
    // TODO reuse restricted points
    _path.reset(path);

    attachSignals(_path.get());
}

void Outline::initPath()
{
    _path.reset(new Path());

    qreal m = 2;
    std::shared_ptr<PathPoint> point1(new PathPoint(m*0, m*0));
    std::shared_ptr<ControlPoint> point2(new ControlPoint(m*16.09549195, m*-31.53267));
    std::shared_ptr<ControlPoint> point3(new ControlPoint(m*70.39944295, m*-113.577872));
    std::shared_ptr<PathPoint> point4(new PathPoint(m*134.750359, m*-114.484482));
    std::shared_ptr<ControlPoint> point5(new ControlPoint(m*148.079229, m*-114.672267));
    std::shared_ptr<ControlPoint> point6(new ControlPoint(m*168.493739, m*-110.447322));
    std::shared_ptr<PathPoint> point7(new PathPoint(m*170.304549, m*-97.240702));
    std::shared_ptr<ControlPoint> point8(new ControlPoint(m*171.482419, m*-88.650189));
    std::shared_ptr<ControlPoint> point9(new ControlPoint(m*134.604629, m*-78.11541));
    std::shared_ptr<PathPoint> point10(new PathPoint(m*123.550789, m*-62.04205));
    std::shared_ptr<ControlPoint> point11(new ControlPoint(m*99.87859895, m*-27.6204));
    std::shared_ptr<ControlPoint> point12(new ControlPoint(m*116.439959, m*0));
    std::shared_ptr<PathPoint> point13(new PathPoint(m*116.439959, m*0));

    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor(*point1, *point13));
    std::shared_ptr<Restrictor> aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    point1->setRestrictor(horizontalAxisRestrictor);
    point4->setRestrictor(aboveHorizontalRestrictor);
    point7->setRestrictor(aboveHorizontalRestrictor);
    point10->setRestrictor(aboveHorizontalRestrictor);
    point13->setRestrictor(horizontalAxisRestrictor);

    _path->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    _path->append(std::shared_ptr<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    _path->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    _path->append(std::shared_ptr<PathItem>(new CubicBezier(point10, point11, point12, point13)));

    // pipe the path signals
    attachSignals(_path.get());
}

void Outline::attachSignals(Path *path)
{
    connect(path, SIGNAL(pathChanged(patheditor::Path*)), this, SLOT(onOutlineChanged()));
    connect(path, SIGNAL(pathReleased(patheditor::Path*)), this, SLOT(onOutlineReleased()));
}

void Outline::onOutlineChanged()
{
    emit outlineChanged(this);
}

void Outline::onOutlineReleased()
{
    emit outlineReleased(this);
}
