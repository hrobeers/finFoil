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

#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QCoreApplication>

QString buildAbout(QString paragraphDelimiter)
{
    return QString(QCoreApplication::tr("finFoil v") +
                   version.toString() +
                   paragraphDelimiter +
                   QCoreApplication::tr("finFoil is an easy to use fin or wing design tool.") +
                   paragraphDelimiter +
                   QCoreApplication::tr("finFoil is licensed under the GNU LGPL version 2.1. "
                      "In Short this means that you can use and distribute the finFoil binaries without restrictions.") +
                   paragraphDelimiter +
                   QCoreApplication::tr("The majority of the finFoil code is licensed under the 2-clause BSD license. "
                      "Meaning that you can reuse these parts of the code under the conditions of the 2-clause BSD license. "
                      "However, since some source files are licensed under the GNU LGPL version 2.1, the entire application inherits this license.") +
                   paragraphDelimiter +
                   QCoreApplication::tr("Copyright 2012-2016 Hans Robeers (hrobeers). All rights reserved."));
}

QString about()
{
    return buildAbout("\n\n");
}

QString aboutHtml()
{
    return QString(QCoreApplication::tr("<h3>About finFoil</h3>") +
                   "</p><p>" +
                   buildAbout("</p><p>") +
                   "</p>");
}

#endif // ABOUT_HPP
