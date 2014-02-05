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
#include "patheditor/pathpoint.h"
#include "patheditor/controlpoint.h"

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
    std::unique_ptr<QObject> o(hrlib::serialization::deserialize(&obj));

    QCOMPARE(o->metaObject()->className(), p.metaObject()->className());

    Testobject *to = (Testobject*)o.get();
    QCOMPARE(to->x(), p.x());
    QCOMPARE(to->y(), p.y());
    QCOMPARE(to->optionalStr(), p.optionalStr());

    Nestedobject *no = to->nestedObj();
    QVERIFY(no != nullptr);
    QCOMPARE(no->someString(), p.nestedObj()->someString());


    //
    // Test optional property deserialization
    //
    QJsonObject pObj = obj[p.metaObject()->className()].toObject();
    pObj.remove("optionalStr");
    obj[p.metaObject()->className()] = pObj;

    std::unique_ptr<Testobject> optionalObj((Testobject*)hrlib::serialization::deserialize(&obj));
    QCOMPARE(optionalObj->optionalStr(), QStringLiteral("initialized"));
}

void SerializationTests::testSerializationFailures()
{
    //
    // Test deserialization failure and error message for empty JSON object
    //
    QJsonObject json;
    QString errorMsg;

    QVERIFY(errorMsg.isEmpty());

    std::unique_ptr<QObject> qObj(hrlib::serialization::deserialize(&json, &errorMsg));

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

    qObj.reset(hrlib::serialization::deserialize(&json, &errorMsg2));

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
    QJsonObject pObj = json2[p.metaObject()->className()].toObject();
    pObj.remove("x");
    json2[p.metaObject()->className()] = pObj;

    qObj.reset(hrlib::serialization::deserialize(&json2, &errorMsg3));

    QVERIFY(qObj == 0);
    QVERIFY(errorMsg2 != errorMsg3);
    QVERIFY(errorMsg3.contains(p.metaObject()->className()));
    QVERIFY(errorMsg3.contains("x"));

    // Test exception version
    QTR_ASSERT_THROW(hrlib::serialization::deserialize(&json2), hrlib::SerializationException)
}

void SerializationTests::testPathPointSerialization()
{
    PathPoint p(1, 2.34);
    QJsonObject json = hrlib::serialization::serialize(&p);
    QJsonObject value = json.value(p.metaObject()->className()).toObject();

    QCOMPARE(value.value("x").toDouble(), 1.0);
    QCOMPARE(value.value("y").toDouble(), 2.34);

    std::unique_ptr<PathPoint> newPP((PathPoint*)hrlib::serialization::deserialize(&json));

    QCOMPARE(newPP->x(), 1.0);
    QCOMPARE(newPP->y(), 2.34);
}

void SerializationTests::testControlPointSerialization()
{
    ControlPoint p(1.23, 4);
    QJsonObject json = hrlib::serialization::serialize(&p);
    QJsonObject value = json.value(p.metaObject()->className()).toObject();

    QCOMPARE(value.value("x").toDouble(), 1.23);
    QCOMPARE(value.value("y").toDouble(), 4.0);

    std::unique_ptr<ControlPoint> newPP((ControlPoint*)hrlib::serialization::deserialize(&json));

    QCOMPARE(newPP->x(), 1.23);
    QCOMPARE(newPP->y(), 4.0);
    QCOMPARE(newPP->metaObject()->className(), "patheditor::ControlPoint");
}

QTR_ADD_TEST(SerializationTests)
