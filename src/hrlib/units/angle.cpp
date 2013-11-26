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

#include "angle.h"

#include <QString>

using namespace hrlib::units;
using namespace boost::units;
using namespace boost::units::degree;

Angle::Angle()
{
    setInternalValue(quantity<plane_angle, qreal>(0 * degrees));
    _displayUnit = Unit::degree;
}

Angle::Angle(boost::units::quantity<degree::plane_angle, qreal> internalValue, Angle::Unit::e displayUnit)
{
    setInternalValue(internalValue);
    _displayUnit = displayUnit;
}

qreal Angle::value()
{
    if (_displayUnit == Unit::degree)
    {
        return _internalValue.value();
    }

    return qreal(0)/qreal(0);
}

void Angle::setValue(qreal value)
{
    if (_displayUnit == Unit::degree)
    {
        quantity<plane_angle, qreal> angle(value * degrees);
        _internalValue = angle;
    }
}

QString Angle::unitSymbol()
{
    return "deg";
}

QString Angle::unitName()
{
    return "";
}

Angle::~Angle()
{
}
