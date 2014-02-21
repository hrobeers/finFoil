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

#define SERIALIZABLE(CLASS, UNIQUE_NAME) Q_DECLARE_METATYPE(CLASS *) \
    namespace serialization_register { /*Avoid name clashes with global variables*/\
        static hrlib::serialization::registerForSerialization<CLASS> UNIQUE_NAME(#UNIQUE_NAME);\
    }

#define CUSTOMSERIALIZABLE(CLASS, UNIQUE_NAME, CUSTOM_SERIALIZER_CLASS) Q_DECLARE_METATYPE(CLASS *) \
    namespace serialization_register { /*Avoid name clashes with global variables*/\
        static const CUSTOM_SERIALIZER_CLASS CUSTOM_SERIALIZER_CLASS;\
        static hrlib::serialization::registerForSerialization<CLASS> UNIQUE_NAME(#UNIQUE_NAME, &CUSTOM_SERIALIZER_CLASS);\
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
    public:
        //
        // Classes for custom serialization
        //

        class ICustomSerializer
        {
        public:
            virtual QJsonObject serialize(const QObject *object) const = 0;
            virtual std::unique_ptr<QObject> deserialize(const QJsonObject *jsonObject, QString *errorMsg = 0) const = 0;

            virtual ~ICustomSerializer() {}
        };

        template <typename T>
        class CustomSerializer : public ICustomSerializer
        {
        protected:
            virtual QJsonObject serializeImpl(const T *object) const = 0;
            virtual std::unique_ptr<T> deserializeImpl(const QJsonObject *jsonObject, QString *errorMsg) const = 0;

        public:
            virtual QJsonObject serialize(const QObject *object) const override final
                { return serializeImpl(qobject_cast<const T*>(object)); }
            virtual std::unique_ptr<QObject> deserialize(const QJsonObject *jsonObject, QString *errorMsg = 0) const override final
                { return deserializeImpl(jsonObject, errorMsg); }

            virtual ~CustomSerializer() {}
        };

    private:
        static QMap<QString, const QObject*>& typeMapPriv()
        {
            static QMap<QString, const QObject*> tMap;
            return tMap;
        }
        static QMap<QString, const ICustomSerializer*>& serializerMapPriv()
        {
            static QMap<QString, const ICustomSerializer*> sMap;
            return sMap;
        }
        static nm_type& nameMapPriv()
        {
            static nm_type  nMap;
            return nMap;
        }

    public:
        // Exception throwing methods
        static QJsonObject serialize(const QObject *qObj);
        static std::unique_ptr<QObject> deserialize(const QJsonObject *jsonObj);
        static std::unique_ptr<QObject> deserializeClass(const QJsonObject *jsonObj, QString className);

        // ErrorMsg methods
        static std::unique_ptr<QObject> deserialize(const QJsonObject *jsonObj, QString *errorMsg);
        static std::unique_ptr<QObject> deserializeClass(const QJsonObject *jsonObj, QString className, QString *errorMsg);

        // Public map getters
        static const QMap<QString, const QObject*>& typeMap() { return typeMapPriv(); }
        static const QMap<QString, const ICustomSerializer*>& serializerMap() { return serializerMapPriv(); }
        static const nm_type& nameMap() { return nameMapPriv(); }

        // Auxilliary methods
        static bool isRegistered(QString *className, QString *errorMsg = 0);
        static QString toSerialName(QString className);
        static QString toClassName(QString serialName);

        // Registration class (Use SERIALIZABLE macro)
        template <typename T>
        class registerForSerialization
        {
        public:
            registerForSerialization(QString serialName, const ICustomSerializer* serializer = nullptr)
            {
                static const T t;
                typeMapPriv()[t.metaObject()->className()] = &t;
                nameMapPriv().insert(nm_type::value_type(t.metaObject()->className(), serialName));
                if (serializer) serializerMapPriv()[t.metaObject()->className()] = serializer;
                qRegisterMetaType<T*>();
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
