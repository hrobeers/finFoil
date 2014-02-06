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

#ifndef SERIALIZATIONTESTS_H
#define SERIALIZATIONTESTS_H

#include <QObject>
#include <QUuid>
#include "serialization/serialization.h"

class SerializationTests : public QObject
{
    Q_OBJECT

private slots:
    void testSerialization();
    void testSerializationFailures();

    void testPathPointSerialization();
    void testControlPointSerialization();
};

class Nestedobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString someString READ someString WRITE setSomeString)
    Q_PROPERTY(int random READ random)

private:
    QString _someString;
    int _random;

public:
    Q_INVOKABLE Nestedobject() { _random = (int)this; }

    virtual ~Nestedobject() {}

    QString someString() const { return _someString; }
    int random() const { return _random; }

    void setSomeString(const QString &someString) { _someString = someString; }
};
DESERIALIZABLE(Nestedobject, REG_NESTEDOBJECT);

class SingleProperty : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QUuid someUuid READ someUuid WRITE setSomeUuid)

private:
    QUuid _someUuid;

public:
    Q_INVOKABLE SingleProperty() { _someUuid = QUuid::createUuid(); }

    virtual ~SingleProperty() {}

    QUuid someUuid() const { return _someUuid; }

    void setSomeUuid(const QUuid &someUuid) { _someUuid = someUuid; }
};
DESERIALIZABLE(SingleProperty, REG_SINGLEPROPERTY);

class Testobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(QString optionalStr READ optionalStr WRITE setOptionalStr RESET initOptionalStr)
    Q_PROPERTY(Nestedobject* nestedObj READ nestedObj WRITE setNestedObj)
    Q_PROPERTY(SingleProperty* singleProp READ singleProp WRITE setSingleProp)

private:
    qreal _x, _y;
    QString _optionalStr;
    Nestedobject *_nestedObj;
    SingleProperty *_sProp;

    void init()
    {
        _nestedObj = new Nestedobject();
        _nestedObj->setParent(this);

        _sProp = new SingleProperty();
        _sProp->setParent(this);
    }

public:
    Q_INVOKABLE Testobject() : _x(0), _y(0), _optionalStr("") { init(); }
    Testobject(qreal x, qreal y) : _x(x), _y(y), _optionalStr("") { init(); }

    // Q_PROPERTY getters
    qreal x() const { return _x; }
    qreal y() const { return _y; }
    QString optionalStr() const { return _optionalStr; }
    Nestedobject* nestedObj() const { return _nestedObj; }
    SingleProperty* singleProp() const { return _sProp; }

    // Q_PROPERTY setters
    void setX(const qreal x) { _x = x; }
    void setY(const qreal y) { _y = y; }
    void setOptionalStr(const QString &str) { _optionalStr = str; }
    void setNestedObj(Nestedobject *nObj) { _nestedObj = nObj; }
    void setSingleProp(SingleProperty *sProp) { _sProp = sProp; }

    // Q_PROPERTY resetters
    void initOptionalStr() { _optionalStr = "initialized"; }
};
DESERIALIZABLE(Testobject, REG_TESTOBJECT);

#endif // SERIALIZATIONTESTS_H
