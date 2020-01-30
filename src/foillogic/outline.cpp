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

#include "foillogic/outline.hpp"

#include "patheditor/path.hpp"
#include "patheditor/controlpoint.hpp"
#include "patheditor/curvepoint.hpp"
#include "patheditor/cubicbezier.hpp"
#include "patheditor/pointrestrictor.hpp"
#include "patheditor/linerestrictor.hpp"
#include "patheditor/quadrantrestrictor.hpp"

SERIALIZABLE(foillogic::Outline, outline)

using namespace foillogic;
using namespace patheditor;
using namespace boost::units;

Outline::Outline(QObject *parent) :
    QObject(parent),
    _height(0.1 * si::meter), // 10cm
    _area(0 * si::meter * si::meter)
{
    initPath();
}

Path *Outline::path()
{
    return _path.get();
}

quantity<si::length, qreal> Outline::height() const
{
    return _height;
}

quantity<si::area, qreal> Outline::area() const
{
    return _area;
}

boost::units::quantity<si::plane_angle, qreal> Outline::sweep() const
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
    assert(path->pathItems().count()!=0);

    std::shared_ptr<Restrictor> originRestrictor(new PointRestrictor({0,0}));
    std::shared_ptr<Restrictor> horizontalAxisRestrictor(new LineRestrictor({0,0},{1,0}));
    //std::shared_ptr<Restrictor> aboveHorizontalRestrictor(new QuadrantRestrictor(Quadrants::I | Quadrants::II));

    path->pathItems().first()->startPoint()->setRestrictor(originRestrictor);
    path->pathItems().last()->endPoint()->setRestrictor(horizontalAxisRestrictor);

    if (_path) _path->disconnect();
    _path.reset(path);
    attachSignals(_path.get());
}

Outline::~Outline() {}

void Outline::initPath()
{
    if (_path) _path->disconnect();
    std::unique_ptr<Path> path(new Path());

    qreal m = 2;
    qshared_ptr<CurvePoint> point1 = make_qshared(new CurvePoint(m*0, m*0));
    qshared_ptr<ControlPoint> point2 = make_qshared(new ControlPoint(m*16.095, m*-31.532));
    qshared_ptr<ControlPoint> point3 = make_qshared(new ControlPoint(m*70.399, m*-113.57));
    qshared_ptr<CurvePoint> point4 = make_qshared(new CurvePoint(m*134.75, m*-114.48));
    qshared_ptr<ControlPoint> point5 = make_qshared(new ControlPoint(m*148.07, m*-114.67));
    qshared_ptr<ControlPoint> point6 = make_qshared(new ControlPoint(m*158.49, m*-110.44));
    qshared_ptr<CurvePoint> point7 = make_qshared(new CurvePoint(m*160.30, m*-97.24));
    qshared_ptr<ControlPoint> point8 = make_qshared(new ControlPoint(m*151.48, m*-88.650));
    qshared_ptr<ControlPoint> point9 = make_qshared(new ControlPoint(m*134.60, m*-78.115));
    qshared_ptr<CurvePoint> point10 = make_qshared(new CurvePoint(m*123.55, m*-62.042));
    qshared_ptr<ControlPoint> point11 = make_qshared(new ControlPoint(m*99.878, m*-27.620));
    qshared_ptr<ControlPoint> point12 = make_qshared(new ControlPoint(m*113.43, m*-6.5));
    qshared_ptr<CurvePoint> point13 = make_qshared(new CurvePoint(m*116.43, m*0));

    path->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point10, point11, point12, point13)));

    // set the path
    pSetPath(path.release());

    point4->setContinuous(true);
    point10->setContinuous(true);
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
