/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
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

#include "exceptions.h"

#include <QObject>

using namespace hrlib;

Exception::Exception(QObject *thrower) throw()
{
    QString prefix("Exception thrown in ");
    _message = prefix.append(thrower->metaObject()->className());
    _innerException = NULL;
}

Exception::Exception(QString &message, QObject *thrower) throw()
{
    setMessage(message, thrower);
    _innerException = NULL;
}

Exception::Exception(QString &message, exception &innerException, QObject *thrower) throw()
{
    setMessage(message, thrower);
    _innerException = &innerException;
}

void Exception::setMessage(QString &message, QObject *thrower) throw()
{
    if (thrower != NULL)
    {
        QString prefix("Exception thrown in ");
        prefix.append(thrower->metaObject()->className());
        prefix.append(": ");
        message.prepend(prefix);
    }

    _message = message;
}

const QString &Exception::message() const
{
    return _message;
}

const std::exception &Exception::innerException() const
{
    return *_innerException;
}


ArgumentException::ArgumentException(QString &message, QObject *thrower) throw()
    : Exception(message, thrower) { }
