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

#ifndef HRLIB_SERIALIZATION_H
#define HRLIB_SERIALIZATION_H

#define DESERIALIZABLE(CLASS, UNIQUE_NAME) Q_DECLARE_METATYPE(CLASS *) \
    namespace serialization_register { /*Avoid name clashes with global variables*/\
        static hrlib::serialization::registerForDeserialization<CLASS> UNIQUE_NAME(#UNIQUE_NAME);\
    }


#include <QObject>
#include <QJsonObject>
#include <QMetaProperty>
#include "boost/bimap.hpp"
#include "exceptions.h"

namespace hrlib
{
    typedef boost::bimap<QString, QString> nm_type;

    class serialization
    {
    private:
        static QMap<QString, const QObject*>& typeMap()
        {
            static QMap<QString, const QObject*> tMap;
            return tMap;
        }
        static nm_type& nameMap()
        {
            static nm_type  nMap;
            return nMap;
        }

        static bool findClass(const QJsonObject *jsonObj, QString *className, QString *errorMsg);
        static bool isRegistered(QString *className, QString *errorMsg);

    public:
        static QJsonObject serialize(const QObject *qObj);
        static QObject *deserialize(const QJsonObject *jsonObj);
        static QObject *deserializeClass(const QJsonObject *jsonObj, QString className);

        static QObject *deserialize(const QJsonObject *jsonObj, QString *errorMsg);
        static QObject *deserializeClass(const QJsonObject *jsonObj, QString className, QString *errorMsg);

        static QString toSerialName(const QString &className)
        {
            if (nameMap().left.find(className) == nameMap().left.end())
                return className;
            return nameMap().left.at(className);
        }
        static QString toClassName(const QString &serialName)
        {
            if (nameMap().right.find(serialName) == nameMap().right.end())
                return serialName;
            return nameMap().right.at(serialName);
        }

        template <typename T>
        class registerForDeserialization
        {
        public:
            registerForDeserialization(QString serialName)
            {
                static const T t;
                typeMap()[t.metaObject()->className()] = &t;
                nameMap().insert(nm_type::value_type(t.metaObject()->className(), serialName));
            }
        };
    };

    class SerializationException : public Exception
    {
    public:
        explicit SerializationException(QString &message, QObject *thrower = 0) throw()
            : Exception(message, thrower) { }

        virtual ~SerializationException() throw() {}
    };
}

#endif // HRLIB_SERIALIZATION_H
