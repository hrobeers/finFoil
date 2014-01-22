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

#ifndef SERIALIZABLE_HPP
#define SERIALIZABLE_HPP

#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include "exceptions.h"

namespace hrlib
{
    //
    // Curiously recurring template pattern
    // To fake static virtual methods
    //

    template <typename Derived>
    class Serializable
    {
    public:
        static QJsonObject serialize(Serializable* obj)
        {
            // serializeImpl should be implemented in Derived class
            return Derived::serializeImpl(static_cast<Derived*>(obj));
        }

        static Derived* deserialize(QJsonObject obj)
        {
            // deserializeImpl should be implemented in Derived class
            return Derived::deserializeImpl(obj);
        }

        QJsonObject serialize()
        {
            return serialize(this);
        }
    };

    class SerializationException : public Exception
    {
    public:
        explicit SerializationException(QString &message, QObject *thrower = 0) throw()
            : Exception(message, thrower) { }

        virtual ~SerializationException() throw() {}
    };
}

#endif // SERIALIZABLE_HPP
