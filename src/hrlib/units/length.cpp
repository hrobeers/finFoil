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

#include "length.h"

#include <QString>
#include "boost/units/base_units/cgs/centimeter.hpp"
#include "boost/units/systems/cgs/length.hpp"

using namespace hrlib::units;
using namespace boost::units;

Length::Length()
{
    setInternalValue(quantity<si::length, qreal>(0 * si::meter));
    _displayUnit = Unit::m;
}

Length::Length(boost::units::quantity<si::length, qreal> internalValue, Unit::e displayUnit)
{
    setInternalValue(internalValue);
    _displayUnit = displayUnit;
}

qreal Length::value()
{
    if (_displayUnit == Unit::m)
    {
        return _internalValue.value();
    }
    else if (_displayUnit == Unit::cm)
    {
        quantity<cgs::length, qreal> converted(_internalValue);
        return converted.value();
    }

    return qreal(0)/qreal(0);
}

void Length::setValue(qreal value)
{
    if (_displayUnit == Unit::m)
    {
        quantity<si::length, qreal> m(value * si::meter);
        _internalValue = m;
    }
    else if (_displayUnit == Unit::cm)
    {
        quantity<si::length, qreal> cm(value * cgs::centimeter);
        _internalValue = cm;
    }
}

QString Length::unitSymbol()
{
    switch (_displayUnit) {
    case Unit::m:
        return "m";

    case Unit::cm:
        return "cm";

    default:
        return "";
    }
}

QString Length::unitName()
{
    return "";
}

Length::~Length()
{
}
