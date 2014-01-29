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

class Testobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(QString str READ str WRITE setStr RESET initStr)

private:
    qreal _x, _y;
    QString _optionalStr;

public:
    Q_INVOKABLE Testobject() : _x(0), _y(0), _optionalStr("") {}
    Testobject(qreal x, qreal y) : _x(x), _y(y), _optionalStr("") {}

    qreal x() { return _x; }
    qreal y() { return _y; }
    QString str() { return _optionalStr; }

    void setX(const qreal x) { _x = x; }
    void setY(const qreal y) { _y = y; }
    void setStr(const QString &str) { _optionalStr = str; }

    void initStr() { _optionalStr = "initialized"; }
};

#endif // SERIALIZATIONTESTS_H
