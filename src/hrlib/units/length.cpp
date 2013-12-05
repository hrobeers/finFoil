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
#include "boost/units/systems/cgs/length.hpp"
#include "boost/mpl/string.hpp"
#include "hrlib/units/systems.h"

using namespace hrlib::units;
using namespace boost::units;
using namespace boost::mpl;

static const UnitConvertor<si::length, si::length, string<'m'> > CONVERTOR_M;
static const UnitConvertor<cgs::length, si::length, string<'cm'> > CONVERTOR_CM;
static const UnitConvertor<ft::length, si::length, string<'ft'> > CONVERTOR_FT;
static const UnitConvertor<inch::length, si::length, string<'in'> > CONVERTOR_IN;

static const Length::insertConvertor m(Length::Unit::m, &CONVERTOR_M);
static const Length::insertConvertor cm(Length::Unit::cm, &CONVERTOR_CM);
static const Length::insertConvertor ft(Length::Unit::ft, &CONVERTOR_FT);
static const Length::insertConvertor in(Length::Unit::m, &CONVERTOR_IN);

Length::Length() :
    QuantityBase(quantity<si::length, qreal>(0 * si::meter), Unit::cm)
{
}

Length::Length(boost::units::quantity<si::length, qreal> internalValue, Unit::e /*unused*/) :
    QuantityBase(internalValue, Unit::cm)
{
}
