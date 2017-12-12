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
#include <QJsonDocument>
#include "foillogic/foil.hpp"
#include "foillogic/outline.hpp"
#include "foillogic/profile.hpp"
#include "foillogic/thicknessprofile.hpp"
#include "patheditor/path.hpp"
#include "patheditor/line.hpp"
#include "patheditor/cubicbezier.hpp"
#include "patheditor/pathpoint.hpp"
#include "patheditor/controlpoint.hpp"
#include "patheditor/curvepoint.hpp"

using namespace patheditor;
using namespace foillogic;
using namespace jenson;

void SerializationTests::testFoilSerialization()
{
    QString errorMsg;
    std::unique_ptr<Foil> foil(new Foil());

    QJsonObject serialized = JenSON::serialize(foil.get());
    auto deserialized = JenSON::deserialize<Foil>(&serialized, &errorMsg);

    if (!deserialized)
        QFAIL(errorMsg.toUtf8().constData());
}

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

    point4->setContinuous(true);

    QJsonObject obj = JenSON::serialize(path.get());

    qunique_ptr<Path> deserialized = JenSON::deserialize<Path>(&obj);

    // Test PathItem count
    QVERIFY(deserialized->pathItems().count() > 0);
    QCOMPARE(deserialized->pathItems().count(), path->pathItems().count());
    // Compare startPoints of last pathItems
    QCOMPARE(*deserialized->pathItems().last()->constStartPoint(), *path->pathItems().last()->constStartPoint());
    // Make sure that point to different objects
    QVERIFY(deserialized->pathItems().last()->constStartPoint() != path->pathItems().last()->constStartPoint());

    // Test continuity serialization (only point4 is set continuous)
    QVERIFY(!deserialized->pathItems().first()->constStartPoint()->continuous());
    QVERIFY( deserialized->pathItems().first()->constEndPoint()->continuous());
    QVERIFY(!deserialized->pathItems().last()->constStartPoint()->continuous());
}

void SerializationTests::testFlagSerialization()
{
    qunique_ptr<Profile> profile(new Profile());
    QVERIFY(profile->editable());

    QJsonObject serialized = JenSON::serialize(profile.get());
    qunique_ptr<Profile> deserialized = JenSON::deserialize<Profile>(&serialized);
    QVERIFY(deserialized->editable());

    profile->setEditable(false);
    QJsonObject serialized2 = JenSON::serialize(profile.get());
    qunique_ptr<Profile> deserialized2 = JenSON::deserialize<Profile>(&serialized2);
    QVERIFY(!deserialized2->editable());
}

///
/// \brief SerializationTests::testDeserializing_v_1_0
///
/// Paths received an added cont property to highlight continuity.
/// This change should be backwards compatible, meaning that old files should still load successfully.
///
void SerializationTests::testDeserializing_v_1_0()
{
    QString errorMsg;
    QByteArray foil("{\"foil\":{\"history\":[],\"layerCount\":7,\"outline\":{\"area\":0.007457491099408858,\"height\":0.10000000000000001,\"path\":[[\"M\",0,0],[\"C\",32.190983899999999,-63.065339999999999,140.79888589999999,-227.155744,269.50071800000001,-228.968964],[\"C\",296.158458,-229.34453400000001,336.98747800000001,-220.894644,340.60909800000002,-194.481404],[\"C\",342.96483799999999,-177.30037799999999,238.20925799999998,-115.23081999999999,216.10157799999999,-83.084100000000007],[\"C\",168.75719790000002,-14.240800000000007,226.879918,-13,232.879918,0]],\"sweep\":0.72127218250220537,\"uuid\":\"{a181c1db-311c-4cee-a715-d30eb0ea0f2e}\"},\"profile\":{\"botProfile\":[[\"M\",0,0],[\"C\",0,0,-6,15,84,15],[\"C\",128,16,300,0,300,0]],\"minThick\":0.001,\"symmetry\":\"Asymmetric\",\"thickness\":0.01,\"thicknessRatio\":2.3263550082302116,\"topProfile\":[[\"M\",0,0],[\"C\",0,0,0,-35,90,-35],[\"C\",135,-35,300,0,300,0]],\"uuid\":\"{698a3768-a4d9-4e01-8a4b-710c4ea86d74}\"},\"thickness\":{\"topProfile\":[[\"M\",0,-30],[\"C\",0,-30,300,-30,300,0]],\"uuid\":\"{2d5cced3-5411-4b0a-93a3-494251844f66}\"},\"uuid\":\"{d4ad1241-0712-4931-beb5-31c231108348}\"}}");

    QJsonObject obj = QJsonDocument::fromJson(foil).object();
    qunique_ptr<Foil> deserialized = JenSON::deserialize<Foil>(&obj, &errorMsg);

    // Check if deserialization succeeded
    if (!deserialized)
        QFAIL(errorMsg.toUtf8().constData());

    // Check if path properties match
    QCOMPARE(deserialized->outline()->path()->pathItems().count(), 4);
    QCOMPARE(deserialized->thicknessProfile()->pTopProfile()->pathItems().count(), 1);
    QCOMPARE(deserialized->profile()->pTopProfile()->pathItems().count(), 2);
    QCOMPARE(deserialized->profile()->pBotProfile()->pathItems().count(), 2);
    QCOMPARE(deserialized->profile()->symmetry(), Profile::Asymmetric);
    QCOMPARE(deserialized->pThickness(), 0.01);

    // Verify that the v1.0 thickness property is not present in the current serialization
    QByteArray serialized = QJsonDocument(JenSON::serialize(deserialized.get())).toJson(QJsonDocument::Compact);
    QVERIFY(!serialized.contains("\"thickness\":0"));

    // A flags property is added to v1.2, make sure flag properties are initialized correctly
    QVERIFY(deserialized->profile()->editable());
}

QTR_ADD_TEST(SerializationTests)
