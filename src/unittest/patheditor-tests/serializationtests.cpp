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

#include <QJsonArray>
#include <memory>

#include "patheditor/path.h"
#include "patheditor/line.h"
#include "patheditor/cubicbezier.h"

using namespace patheditor;

void SerializationTests::testSerialization()
{
    //
    // Test serialization
    //
    Testobject p(2, 3);
    p.setOptionalStr("This is a Testobject");
    p.nestedObj()->setSomeString("This is a nested object");

    QJsonObject obj = hrlib::serialization::serialize(&p);

    //
    // Test deserialization
    //
    std::unique_ptr<QObject> o = hrlib::serialization::deserializeToObject(&obj);

    QCOMPARE(o->metaObject()->className(), p.metaObject()->className());

    Testobject *to = (Testobject*)o.get();

    // test members
    QCOMPARE(to->x(), p.x());
    QCOMPARE(to->y(), p.y());
    QCOMPARE(to->optionalStr(), p.optionalStr());

    // test single property nested object
    QCOMPARE(to->singleProp()->someUuid(), p.singleProp()->someUuid());

    // test Lists
    QCOMPARE(to->internalList()->first()->someUuid(), p.internalList()->first()->someUuid());
    QVERIFY(to->internalList()->at(0)->metaObject()->className() != to->internalList()->at(1)->metaObject()->className());
    QVERIFY(to->internalList()->at(0)->metaObject()->className() == to->internalList()->at(2)->metaObject()->className());
    QCOMPARE(to->intList().last(), p.intList().last());


    // test nested object
    Nestedobject *no = to->nestedObj();
    QVERIFY(no != nullptr);
    QCOMPARE(no->someString(), p.nestedObj()->someString());


    //
    // Test optional property deserialization
    //
    QJsonObject pObj = obj[hrlib::serialization::toSerialName(p.metaObject()->className())].toObject();
    pObj.remove("optionalStr");
    obj[hrlib::serialization::toSerialName(p.metaObject()->className())] = pObj;

    std::unique_ptr<Testobject> optionalObj = hrlib::serialization::deserialize<Testobject>(&obj);
    QCOMPARE(optionalObj->optionalStr(), QStringLiteral("initialized"));
}

void SerializationTests::testCustomSerialization()
{
    // Test a plain custom serializable class
    CustomSerializable custom;
    QJsonObject jObj = hrlib::serialization::serialize(&custom);

    std::unique_ptr<CustomSerializable> deserialized = hrlib::serialization::deserialize<CustomSerializable>(&jObj);
    QCOMPARE(deserialized->x, (custom.x / 2) + 5);

    // Test a nested custom serializable class
    CustomContainer cont;
    cont.setNested(deserialized.release());
    jObj = hrlib::serialization::serialize(&cont);

    std::unique_ptr<CustomContainer> dCont = hrlib::serialization::deserialize<CustomContainer>(&jObj);
    QCOMPARE(dCont->nested()->x, ((custom.x / 2) + 5) / 2 + 5);
}

void SerializationTests::testSerializationFailures()
{
    //
    // Test deserialization failure and error message for empty JSON object
    //
    QJsonObject json;
    QString errorMsg;

    QVERIFY(errorMsg.isEmpty());

    std::unique_ptr<QObject> qObj = hrlib::serialization::deserializeToObject(&json, &errorMsg);

    QVERIFY(qObj == 0);
    QVERIFY(!errorMsg.isEmpty());

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserializeToObject(&json), hrlib::SerializationException)


    //
    // Test deserialization failure and error message for unkown JSON object
    //
    QString errorMsg2;
    QString className("NotRegisteredClass");
    json.insert(className, QJsonValue());

    qObj = hrlib::serialization::deserializeToObject(&json, &errorMsg2);

    QVERIFY(qObj == 0);
    QVERIFY(errorMsg != errorMsg2);
    QVERIFY(errorMsg2.contains(className));

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserializeToObject(&json), hrlib::SerializationException)


    //
    // Test deserialization failure on non-resetable missing field
    //
    QString errorMsg3;
    Testobject p(0.2, -5.3);
    QJsonObject json2 = hrlib::serialization::serialize(&p);
    QJsonObject pObj = json2[hrlib::serialization::toSerialName(p.metaObject()->className())].toObject();
    pObj.remove("x");
    json2[hrlib::serialization::toSerialName(p.metaObject()->className())] = pObj;

    qObj = hrlib::serialization::deserializeToObject(&json2, &errorMsg3);

    QVERIFY(qObj == 0);
    QVERIFY(errorMsg2 != errorMsg3);
    QVERIFY(errorMsg3.contains("x"));

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserializeToObject(&json2), hrlib::SerializationException)


    //
    // Test exception on cast failure
    //
    QJsonObject json3 = hrlib::serialization::serialize(&p);
    QTR_ASSERT_THROW(std::unique_ptr<Nestedobject> invalidCast = hrlib::serialization::deserialize<Nestedobject>(&json3), hrlib::SerializationException)
}

void SerializationTests::testPathSerialization()
{
    std::unique_ptr<Path> path(new Path());

    qreal m = 2;
    std::shared_ptr<PathPoint> point1(new PathPoint(m*0, m*0));
    std::shared_ptr<ControlPoint> point2(new ControlPoint(m*16.09549195, m*-31.53267));
    std::shared_ptr<ControlPoint> point3(new ControlPoint(m*70.39944295, m*-113.577872));
    std::shared_ptr<PathPoint> point4(new PathPoint(m*134.750359, m*-114.484482));
    std::shared_ptr<PathPoint> point7(new PathPoint(m*170.304549, m*-97.240702));
    std::shared_ptr<ControlPoint> point8(new ControlPoint(m*171.482419, m*-88.650189));
    std::shared_ptr<ControlPoint> point9(new ControlPoint(m*134.604629, m*-78.11541));
    std::shared_ptr<PathPoint> point10(new PathPoint(m*123.550789, m*-62.04205));

    path->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    path->append(std::shared_ptr<PathItem>(new Line(point4, point7)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));

    QJsonObject obj = hrlib::serialization::serialize(path.get());

    std::unique_ptr<Path> deserialized = hrlib::serialization::deserialize<Path>(&obj);

    // Test PathItem count
    QVERIFY(deserialized->pathItems().count() > 0);
    QCOMPARE(deserialized->pathItems().count(), path->pathItems().count());
    // Compare startPoints of last pathItems
    QCOMPARE(*deserialized->pathItems().last()->constStartPoint(), *path->pathItems().last()->constStartPoint());
    // Make sure that point to different objects
    QVERIFY(deserialized->pathItems().last()->constStartPoint() != path->pathItems().last()->constStartPoint());
}

QTR_ADD_TEST(SerializationTests)
