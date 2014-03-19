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

#include "serialization.h"

#include <memory>
#include <QStringList>
#include <QJsonArray>

using namespace hrlib;


//
// Private methods declared here to keep header file clean
//

static bool findClass(const QJsonObject *jsonObj, QString *className, QString *errorMsg)
{
    int keyCount = jsonObj->keys().count();
    if (keyCount == 1)
    {
        QString type = serialization::toClassName(jsonObj->keys().first());

        if (serialization::isRegistered(&type, errorMsg))
        {
            *className = type;
            return true;
        }
    }
    else if (errorMsg)
    {
        if (keyCount < 1)
            errorMsg->append("\n Empty json object");
        else
            errorMsg->append("\n JsonObj contains multiple keys");
    }

    return false;
}

static QJsonValue serialize(const QVariant var, bool *ok)
{
    QJsonValue v;
    QObject *nestedObj = nullptr;
    QJsonObject nestedJSON;
    QList<QVariant> varList;
    QJsonArray jsArray;

    switch (var.type())
    {
    case QVariant::Invalid:
        *ok = false;
        break;

    case QVariant::UserType:
        nestedObj = qvariant_cast<QObject*>(var);
        if (nestedObj)
        {
            *ok = true;
            nestedJSON = serialization::serialize(nestedObj);
            v = nestedJSON.value(nestedJSON.keys().first());
        }
        break;

    case QVariant::StringList:
        varList = var.toList();

        foreach (QVariant lvar, varList)
            jsArray.append(QJsonValue::fromVariant(lvar));

        v = jsArray;
        *ok = !v.isNull();
        break;

    case QVariant::List:
        varList = var.toList();
        foreach (QVariant lvar, varList)
        {
            QJsonObject listItem;

            QObject *qObj = qvariant_cast<QObject*>(lvar);
            if (qObj)
                listItem.insert(serialization::toSerialName(qObj->metaObject()->className()), serialize(lvar, ok));
            else
                listItem.insert(serialization::toSerialName(lvar.typeName()), serialize(lvar, ok));

            jsArray.append(listItem);

            if (!*ok)
                break;
        }
        v = jsArray;
        *ok = !v.isNull();
        break;

    default:
        v = QJsonValue::fromVariant(var);

        if (!v.isNull())
        {
            *ok = true;
        }
        else
        {
            QString msg("Serialization::serialize not implemented for ");
            msg.append(var.typeName());
            throw NotImplementedException(msg);
        }

        break;
    }
    return v;
}


//
// serialization static class methods
//

QJsonObject serialization::serialize(const QObject *qObj)
{
    QJsonObject retVal; // return value
    QJsonValue value;

    QString className = qObj->metaObject()->className();
    QString serialName = toSerialName(className);

    if (serializerMap().contains(className))
    {
        value = serializerMap()[className]->serialize(qObj);
    }
    else
    {
        QJsonObject propObj; // QProperties container

        // The first propetry objectName is skipped
        for (int i = 1; i < qObj->metaObject()->propertyCount(); i++)
        {
            QMetaProperty mp = qObj->metaObject()->property(i);

            QVariant var = mp.read(qObj);

            bool ok = false;

            QJsonValue v = ::serialize(var, &ok);

            if (!ok)
                continue;

            propObj.insert(mp.name(), v);
        }

        value = propObj;
    }

    retVal.insert(serialName, value);

    return retVal;
}

std::unique_ptr<QObject> serialization::deserializeToObject(const QJsonObject *jsonObj)
{
    QString errorMsg;
    std::unique_ptr<QObject> retVal = deserializeToObject(jsonObj, &errorMsg);

    if (!retVal)
        throw SerializationException(errorMsg);

    return retVal;
}

std::unique_ptr<QObject> serialization::deserializeClass(const QJsonObject *jsonObj, QString className)
{
    QString errorMsg;
    std::unique_ptr<QObject> retVal = deserializeClass(jsonObj, className, &errorMsg);

    if (!retVal)
        throw SerializationException(errorMsg);

    return retVal;
}

std::unique_ptr<QObject> serialization::deserializeToObject(const QJsonObject *jsonObj, QString *errorMsg)
{
    QString className;

    if (!findClass(jsonObj, &className, errorMsg))
        return nullptr;

    // Extract the class data
    QJsonValue classValue = jsonObj->value(toSerialName(className));

    // Use custom deserializer if available
    if (serializerMap().contains(className))
        return serializerMap()[className]->deserialize(&classValue, errorMsg);

    QJsonObject classDataObject = classValue.toObject();
    return deserializeClass(&classDataObject, className, errorMsg);
}

std::unique_ptr<QObject> serialization::deserializeClass(const QJsonObject *jsonObj, QString className, QString *errorMsg)
{
    className = className.replace('*', ""); // Properties can be pointer types

    if (!isRegistered(&className, errorMsg))
        return nullptr;

    const QObject *obj = typeMap()[className];
    std::unique_ptr<QObject> retVal(obj->metaObject()->newInstance());
    if (!retVal)
    {
        QString msg = "serialization::deserialize failed for " + className +
                ": the default ctor is not invokable. Add the Q_INVOKABLE macro.";
        throw ImplementationException(msg);
    }

    // Loop over and write class properties
    // The first propetry objectName is skipped
    for (int i = 1; i < retVal->metaObject()->propertyCount(); i++)
    {
        QMetaProperty mp = retVal->metaObject()->property(i);
        QString propName = mp.name();
        className = mp.typeName();
        className = className.replace('*', "");

        if (mp.isWritable())
        {
            // init local variables
            QObject *nestedObj = nullptr;
            QJsonObject nestedJSON;
            QJsonValue nestedJsonValue;
            QJsonArray jsonArray;
            QVariant var;
            QList<QVariant> varList;
            QStringList stringList;
            bool writeSucceeded = false;

            switch (mp.type())
            {
            case QVariant::UserType:
                nestedJsonValue = jsonObj->value(propName);
                nestedJSON = nestedJsonValue.toObject();

                // Use custom deserializer if available
                if (serializerMap().contains(className))
                {
                    nestedObj = serializerMap()[className]->deserialize(&nestedJsonValue, errorMsg).release();
                }
                else
                {
                    // get className from nestedJSON if specified
                    findClass(&nestedJSON, &className, nullptr);

                    nestedObj = deserializeClass(&nestedJSON, className, errorMsg).release();
                }

                if (nestedObj)
                {
                    nestedObj->setParent(retVal.get());
                    var.setValue(nestedObj);
                    writeSucceeded = mp.write(retVal.get(), var);
                }
                break;

            case QVariant::StringList:
                jsonArray = jsonObj->value(propName).toArray();

                foreach (QJsonValue item, jsonArray)
                    stringList.append(item.toString());

                writeSucceeded = mp.write(retVal.get(), stringList);
                break;

            case QVariant::List:
                jsonArray = jsonObj->value(propName).toArray();
                foreach (QJsonValue item, jsonArray)
                {
                    QVariant vObj;
                    nestedJSON = item.toObject();

                    // deserialize QVariant supported type
                    QString firstKey = nestedJSON.keys().first();
                    int typeId = QVariant::nameToType(firstKey.toStdString().c_str());
                    if (typeId != QVariant::Invalid && typeId != QVariant::UserType)
                    {
                        QJsonValue val = nestedJSON.value(firstKey);
                        if (!val.isNull())
                        {
                            varList.append(val.toVariant());
                            continue;
                        }
                    }

                    // deserialize custom type
                    vObj.setValue(deserializeToObject(&nestedJSON).release());
                    varList.append(vObj);
                }
                writeSucceeded = mp.write(retVal.get(), varList);
                break;

            default:
                writeSucceeded = mp.write(retVal.get(), jsonObj->value(propName).toVariant());
                break;
            }

            if (!writeSucceeded)
            {
                if (mp.isResettable())
                {
                    mp.reset(retVal.get());
                }
                else
                {
                    if (errorMsg)
                    {
                        errorMsg->append("\n Failed to deserialize ");
                        if (!className.isEmpty()) errorMsg->append(className + "::");
                        errorMsg->append(mp.name());
                        errorMsg->append(" of type: ");
                        errorMsg->append(mp.typeName());
                    }
                    return nullptr;
                }
            }
        }

    }

    return retVal;
}

bool serialization::isRegistered(QString *className, QString *errorMsg)
{
    if (!typeMap().contains(*className))
    {
        if (errorMsg)
            errorMsg->append("\n Class \"" + *className + "\" is not registered for deserialization");
        return false;
    }

    return true;
}

QString serialization::toSerialName(QString className)
{
    className = className.replace('*', "");
    if (nameMap().left.find(className) == nameMap().left.end())
        return className;
    return nameMap().left.at(className);
}

QString serialization::toClassName(QString serialName)
{
    serialName = serialName.replace('*', "");
    if (nameMap().right.find(serialName) == nameMap().right.end())
        return serialName;
    return nameMap().right.at(serialName);
}
