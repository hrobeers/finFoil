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
    std::unique_ptr<QObject> o = hrlib::serialization::deserialize(&obj);

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

    std::unique_ptr<Testobject> optionalObj((Testobject*)hrlib::serialization::deserialize(&obj).release());
    QCOMPARE(optionalObj->optionalStr(), QStringLiteral("initialized"));
}

void SerializationTests::testCustomSerialization()
{
    CustomSerializable custom;
    QJsonObject jObj = hrlib::serialization::serialize(&custom);
    qDebug() << jObj;
    std::unique_ptr<CustomSerializable> deserialized((CustomSerializable*)hrlib::serialization::deserialize(&jObj).release());
    QCOMPARE(deserialized->x, (custom.x / 2) + 5);
}

void SerializationTests::testSerializationFailures()
{
    //
    // Test deserialization failure and error message for empty JSON object
    //
    QJsonObject json;
    QString errorMsg;

    QVERIFY(errorMsg.isEmpty());

    std::unique_ptr<QObject> qObj = hrlib::serialization::deserialize(&json, &errorMsg);

    QVERIFY(qObj == 0);
    QVERIFY(!errorMsg.isEmpty());

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserialize(&json), hrlib::SerializationException)


    //
    // Test deserialization failure and error message for unkown JSON object
    //
    QString errorMsg2;
    QString className("NotRegisteredClass");
    json.insert(className, QJsonValue());

    qObj = hrlib::serialization::deserialize(&json, &errorMsg2);

    QVERIFY(qObj == 0);
    QVERIFY(errorMsg != errorMsg2);
    QVERIFY(errorMsg2.contains(className));

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserialize(&json), hrlib::SerializationException)


    //
    // Test deserialization failure on non-resetable missing field
    //
    QString errorMsg3;
    Testobject p(0.2, -5.3);
    QJsonObject json2 = hrlib::serialization::serialize(&p);
    QJsonObject pObj = json2[hrlib::serialization::toSerialName(p.metaObject()->className())].toObject();
    pObj.remove("x");
    json2[hrlib::serialization::toSerialName(p.metaObject()->className())] = pObj;

    qObj = hrlib::serialization::deserialize(&json2, &errorMsg3);

    QVERIFY(qObj == 0);
    QVERIFY(errorMsg2 != errorMsg3);
    QVERIFY(errorMsg3.contains("x"));

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserialize(&json2), hrlib::SerializationException)
}

void SerializationTests::testPathSerialization()
{
    std::unique_ptr<Path> path(new Path());

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

    path->append(std::shared_ptr<PathItem>(new CubicBezier(point1, point2, point3, point4)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point4, point5, point6, point7)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point7, point8, point9, point10)));
    path->append(std::shared_ptr<PathItem>(new CubicBezier(point10, point11, point12, point13)));

    QJsonObject obj = hrlib::serialization::serialize(path.get());

    qDebug() << obj;
}

QTR_ADD_TEST(SerializationTests)
