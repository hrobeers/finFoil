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

#include "area.h"

#include "boost/units/base_units/cgs/centimeter.hpp"
#include "boost/units/systems/cgs/area.hpp"
#include "boost/units/systems/cgs/length.hpp"
#include "boost/units/systems/si/length.hpp"
#include "boost/mpl/string.hpp"
#include "hrlib/units/systems.h"

using namespace hrlib::units;
using namespace boost::units;
using namespace boost::mpl;

static const UnitConvertor<si::area, si::area, string<'m<s','up>2','</su','p>'> > CONVERTOR_M2;
static const UnitConvertor<cgs::area, si::area, string<'cm<s','up>2','</su','p>'> > CONVERTOR_CM2;
static const UnitConvertor<ft::area, si::area, string<'ft<s','up>2','</su','p>'> > CONVERTOR_FT2;
static const UnitConvertor<inch::area, si::area, string<'in<s','up>2','</su','p>'> > CONVERTOR_IN2;

static const Area::insertConvertor m2(AreaUnit::m2, &CONVERTOR_M2);
static const Area::insertConvertor cm2(AreaUnit::cm2, &CONVERTOR_CM2);
static const Area::insertConvertor ft2(AreaUnit::ft2, &CONVERTOR_FT2);
static const Area::insertConvertor in2(AreaUnit::inch2, &CONVERTOR_IN2);

Area::Area() :
    QuantityBase(quantity<si::area>(0 * si::meter * si::meter), AreaUnit::cm2)
{
}

Area::Area(boost::units::quantity<boost::units::si::area, qreal> internalValue, AreaUnit displayUnit) :
    QuantityBase(internalValue, displayUnit)
{
}
