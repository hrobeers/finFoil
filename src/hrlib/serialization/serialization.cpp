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

#include <QJsonArray>
#include <QStringList>
#include "exceptions.h"

using namespace hrlib;


QJsonObject serialization::serialize(const QObject *qObj)
{
    QJsonObject jsonObj; // return value
    QJsonArray propArr; // array of QProperties

    // The first propetry objectName is skipped
    for (int i = 1; i < qObj->metaObject()->propertyCount(); i++)
    {
        QMetaProperty mp = qObj->metaObject()->property(i);
        QJsonObject prop;
        QJsonValue v;

        QVariant var = mp.read(qObj);

        bool ok = false;

        switch (var.type())
        {
        case QVariant::Bool:
            v = var.toBool();
            break;

        case QVariant::Int:
            v = var.toInt(&ok);
            break;

        case QVariant::Double:
            v = var.toDouble(&ok);
            break;

        case QVariant::String:
            v = var.toString();
            ok = true;
            break;

        case QVariant::Invalid:
            ok = false;
            break;

        default:
            QString msg("Serialization::serialize not implemented for ");
            msg.append(var.typeName());
            throw NotImplementedException(msg);
        }

        if (!ok)
            continue;

        prop.insert(mp.name(), v);
        propArr.append(prop);
    }

    jsonObj.insert(qObj->metaObject()->className(), propArr);

    return jsonObj;
}

QObject *serialization::deserialize(const QJsonObject *jsonObj)
{
    if (jsonObj->keys().count() > 0)
    {
        QString className = jsonObj->keys().first();

        if (typeMap().contains(className))
        {
            const QObject *obj = typeMap()[className];
            QObject *retVal = obj->metaObject()->newInstance();
            if (!retVal)
            {
                // TODO throw default ctor not invokable
            }
            return retVal;
        }

        // TODO throw exception
    }

    // TODO throw exception

    return new QObject();
}
