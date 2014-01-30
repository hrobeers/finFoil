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
#include "serialization/serialization.h"

class SerializationTests : public QObject
{
    Q_OBJECT

private slots:
//    void serializePathPoint();
//    void deserializePathPoint();

    void testSerialization();
    void testSerializationFailures();
};

class Nestedobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString someString READ someString WRITE setSomeKey)
    Q_PROPERTY(int random READ random)

private:
    QString _key;
    int _random;

public:
    Q_INVOKABLE Nestedobject() { _random = (int)this; }

    virtual ~Nestedobject() {}

    QString someString() const { return _key; }
    int random() const { return _random; }

    void setSomeKey(const QString &key) { _key = key; }
};
DESERIALIZABLE(Nestedobject, REG_NESTEDOBJECT)

class Testobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(QString str READ str WRITE setStr RESET initStr)
    Q_PROPERTY(Nestedobject* nestedObj READ nestedObj WRITE setNestedObj)

private:
    qreal _x, _y;
    QString _optionalStr;
    Nestedobject *_nestedObj;

    void init()
    {
        _nestedObj = new Nestedobject();
        _nestedObj->setParent(this);
    }

public:
    Q_INVOKABLE Testobject() : _x(0), _y(0), _optionalStr("") { init(); }
    Testobject(qreal x, qreal y) : _x(x), _y(y), _optionalStr("") { init(); }

    // Q_PROPERTY getters
    qreal x() const { return _x; }
    qreal y() const { return _y; }
    QString str() const { return _optionalStr; }
    Nestedobject* nestedObj() const { return _nestedObj; }

    // Q_PROPERTY setters
    void setX(const qreal x) { _x = x; }
    void setY(const qreal y) { _y = y; }
    void setStr(const QString &str) { _optionalStr = str; }
    void setNestedObj(Nestedobject *nObj) { _nestedObj = nObj; }

    // Q_PROPERTY resetters
    void initStr() { _optionalStr = "initialized"; }
};
DESERIALIZABLE(Testobject, REG_TESTOBJECT)

#endif // SERIALIZATIONTESTS_H
