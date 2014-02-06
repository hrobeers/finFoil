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

using namespace hrlib;


QJsonObject serialization::serialize(const QObject *qObj)
{
    QJsonObject retVal; // return value
    QJsonObject propObj; // QProperties container

    // The first propetry objectName is skipped
    for (int i = 1; i < qObj->metaObject()->propertyCount(); i++)
    {
        QMetaProperty mp = qObj->metaObject()->property(i);
        QJsonValue v;

        QVariant var = mp.read(qObj);

        bool ok = false;

        QObject *nestedObj = nullptr;
        QJsonObject nestedJSON;
        switch (var.type())
        {
        case QVariant::Invalid:
            ok = false;
            break;

        case QVariant::UserType:
            nestedObj = qvariant_cast<QObject*>(var);
            if (nestedObj)
            {
                ok = true;
                nestedJSON = serialize(nestedObj);
                v = nestedJSON.value(nestedJSON.keys().first());
            }
            break;

        default:
            v = v.fromVariant(var);

            if (!v.isNull())
            {
                ok = true;
            }
            else
            {
                QString msg("Serialization::serialize not implemented for ");
                msg.append(var.typeName());
                throw NotImplementedException(msg);
            }

            break;
        }

        if (!ok)
            continue;

        propObj.insert(mp.name(), v);
    }

    retVal.insert(qObj->metaObject()->className(), propObj);

    return retVal;
}

QObject *serialization::deserialize(const QJsonObject *jsonObj)
{
    QString errorMsg;
    std::unique_ptr<QObject> retVal(deserialize(jsonObj, &errorMsg));

    if (!retVal)
        throw SerializationException(errorMsg);

    return retVal.release();
}

QObject *serialization::deserializeClass(const QJsonObject *jsonObj, QString className)
{
    QString errorMsg;
    std::unique_ptr<QObject> retVal(deserializeClass(jsonObj, className, &errorMsg));

    if (!retVal)
        throw SerializationException(errorMsg);

    return retVal.release();
}

bool serialization::findClass(const QJsonObject *jsonObj, QString *className, QString *errorMsg)
{
    int keyCount = jsonObj->keys().count();
    if (keyCount == 1)
    {
        *className = jsonObj->keys().first();

        if (isRegistered(className, errorMsg))
            return true;

        className->clear();
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

QObject *serialization::deserialize(const QJsonObject *jsonObj, QString *errorMsg)
{
    QString className;

    if (!findClass(jsonObj, &className, errorMsg))
        return nullptr;

    // Extract the object containing the properties
    QJsonObject propertiesObject = jsonObj->value(className).toObject();

    return deserializeClass(&propertiesObject, className, errorMsg);
}

QObject *serialization::deserializeClass(const QJsonObject *jsonObj, QString className, QString *errorMsg)
{
    className = className.replace('*', ""); // Properties are pointer types

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

        if (mp.isWritable())
        {
            QObject *nestedObj = nullptr;
            QJsonObject nestedJSON;
            QVariant var;
            bool writeSucceeded = false;
            switch (mp.type())
            {
            case QVariant::UserType:
                nestedJSON = jsonObj->value(propName).toObject();

                if (findClass(&nestedJSON, &className, nullptr))
                    nestedObj = deserializeClass(&nestedJSON, className, errorMsg);
                else
                    nestedObj = deserializeClass(&nestedJSON, mp.typeName(), errorMsg);

                if (nestedObj)
                {
                    nestedObj->setParent(retVal.get());
                    var.setValue(nestedObj);
                    writeSucceeded = mp.write(retVal.get(), var);
                }
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

    return retVal.release();
}
