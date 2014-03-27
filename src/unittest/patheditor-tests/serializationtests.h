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
    void testCustomSerialization();
    void testSerializationFailures();

    void testPathSerialization();
};


//
// Classes for testing
//

class CustomSerializable : public QObject
{
    Q_OBJECT

public:
    qreal x;

    CustomSerializable() : x(5) {}
};

class CustomSerializableSerializer : public hrlib::serialization::CustomSerializer<CustomSerializable>
{
protected:
    virtual QJsonValue serializeImpl(const CustomSerializable *object) const override
    {
        QJsonObject retVal;
        QJsonValue val(object->x / 2);
        retVal.insert("custom", val);
        return retVal;
    }
    virtual std::unique_ptr<CustomSerializable> deserializeImpl(const QJsonValue *jsonValue, QString* /*unused*/) const override
    {
        std::unique_ptr<CustomSerializable> retVal(new CustomSerializable());
        qreal x = jsonValue->toObject().value("custom").toDouble() + 5;
        retVal->x = x;
        return retVal;
    }
};
CUSTOMSERIALIZABLE(CustomSerializable, CustomSerializableSerializer, cserial)

class CustomContainer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(CustomSerializable* nested READ nested WRITE setNested)

private:
    std::unique_ptr<CustomSerializable> _nested;

public:
    Q_INVOKABLE CustomContainer() { _nested.reset(new CustomSerializable()); }
    CustomSerializable* nested() { return _nested.get(); }
    void setNested(CustomSerializable* nested) { _nested.reset(nested); }
};
SERIALIZABLE(CustomContainer, cContainer)

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
SERIALIZABLE(Nestedobject, nObj)

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
SERIALIZABLE(SingleProperty, sProp)

class DerivedSingleProperty : public SingleProperty
{
    Q_OBJECT

public:
    Q_INVOKABLE DerivedSingleProperty() : SingleProperty() {}
};
SERIALIZABLE(DerivedSingleProperty, dProp)

class Testobject : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qreal x READ x WRITE setX)
    Q_PROPERTY(qreal y READ y WRITE setY)
    Q_PROPERTY(QString optionalStr READ optionalStr WRITE setOptionalStr RESET initOptionalStr)
    Q_PROPERTY(Nestedobject* nestedObj READ nestedObj WRITE setNestedObj)
    Q_PROPERTY(SingleProperty* singleProp READ singleProp WRITE setSingleProp)
    Q_PROPERTY(QVariantList list READ list WRITE setList)
    Q_PROPERTY(QVariantList intList READ intList WRITE setIntList)

private:
    qreal _x, _y;
    QString _optionalStr;
    Nestedobject *_nestedObj;
    SingleProperty *_sProp;
    QList<std::shared_ptr<SingleProperty>> _list;
    QList<int> _intList;

    void init()
    {
        _nestedObj = new Nestedobject();
        _nestedObj->setParent(this);

        _sProp = new SingleProperty();
        _sProp->setParent(this);

        _list.append(std::shared_ptr<SingleProperty>(new SingleProperty()));
        _list.append(std::shared_ptr<SingleProperty>(new DerivedSingleProperty()));
        _list.append(std::shared_ptr<SingleProperty>(new SingleProperty()));

        _intList.append((int)this);
        _intList.append((int)&_intList);
    }

public:
    Q_INVOKABLE Testobject() : _x(0), _y(0), _optionalStr("") { init(); }
    Testobject(qreal x, qreal y) : _x(x), _y(y), _optionalStr("") { init(); }

    QList<std::shared_ptr<SingleProperty>> *internalList() { return &_list; }

    // Q_PROPERTY getters
    qreal x() const { return _x; }
    qreal y() const { return _y; }
    QString optionalStr() const { return _optionalStr; }
    Nestedobject* nestedObj() { return _nestedObj; }
    SingleProperty* singleProp() { return _sProp; }
    QVariantList list()
    {
        QVariantList retVal;
        foreach (const std::shared_ptr<SingleProperty> &item, _list)
        {
            QVariant var = QVariant::fromValue(item.get());
            retVal.append(var);
        }
        return retVal;
    }
    QVariantList intList()
    {
        QVariantList retVal;
        foreach (int item, _intList) { retVal.append(item); }
        return retVal;
    }

    // Q_PROPERTY setters
    void setX(const qreal x) { _x = x; }
    void setY(const qreal y) { _y = y; }
    void setOptionalStr(const QString &str) { _optionalStr = str; }
    void setNestedObj(Nestedobject *nObj) { _nestedObj = nObj; }
    void setSingleProp(SingleProperty *sProp) { _sProp = sProp; }
    void setList(QVariantList list)
    {
        _list.clear();
        foreach (const QVariant &item, list)
        {
            SingleProperty *obj = qvariant_cast<SingleProperty*>(item);
            _list.append(std::shared_ptr<SingleProperty>(obj));
        }
    }
    void setIntList(QVariantList list)
    {
        _intList.clear();
        foreach (const QVariant &item, list) { _intList.append(item.toInt()); }
    }

    // Q_PROPERTY resetters
    void initOptionalStr() { _optionalStr = "initialized"; }
};
SERIALIZABLE(Testobject, tObj)

#endif // SERIALIZATIONTESTS_H
