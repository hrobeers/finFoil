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

#include "serializationtests.h"
#include "submodules/qtestrunner/qtestrunner.hpp"

#include <memory>
#include "path.h"
#include "line.h"
#include "cubicbezier.h"
#include "pathpoint.h"
#include "controlpoint.h"
#include "curvepoint.hpp"

using namespace patheditor;

void SerializationTests::testPathSerialization()
{
    std::unique_ptr<Path> path(new Path());

    qreal m = 2;
    qshared_ptr<PathPoint> point1(new CurvePoint(m*0, m*0));
    qshared_ptr<ControlPoint> point2(new ControlPoint(m*16.09549195, m*-31.53267));
    qshared_ptr<ControlPoint> point3(new ControlPoint(m*70.39944295, m*-113.577872));
    qshared_ptr<PathPoint> point4(new CurvePoint(m*134.750359, m*-114.484482));
    qshared_ptr<PathPoint> point7(new CurvePoint(m*170.304549, m*-97.240702));
    qshared_ptr<ControlPoint> point8(new ControlPoint(m*171.482419, m*-88.650189));
    qshared_ptr<ControlPoint> point9(new ControlPoint(m*134.604629, m*-78.11541));
    qshared_ptr<PathPoint> point10(new CurvePoint(m*123.550789, m*-62.04205));

    path->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    path->append(std::shared_ptr<PathItem>(new Line(point4, point7)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));

    QJsonObject obj = jenson::JenSON::serialize(path.get());

    qunique_ptr<Path> deserialized = jenson::JenSON::deserialize<Path>(&obj);

    // Test PathItem count
    QVERIFY(deserialized->pathItems().count() > 0);
    QCOMPARE(deserialized->pathItems().count(), path->pathItems().count());
    // Compare startPoints of last pathItems
    QCOMPARE(*deserialized->pathItems().last()->constStartPoint(), *path->pathItems().last()->constStartPoint());
    // Make sure that point to different objects
    QVERIFY(deserialized->pathItems().last()->constStartPoint() != path->pathItems().last()->constStartPoint());
}

QTR_ADD_TEST(SerializationTests)
