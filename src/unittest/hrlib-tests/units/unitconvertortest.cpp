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

#include "unitconvertortest.h"

#include "hrtestlib/unittesting.h"
#include "hrlib/units/unitconvertor.h"
#include "boost/units/systems/si/length.hpp"
#include "boost/units/systems/cgs/length.hpp"

using namespace hrlib::units;
using namespace boost::units;

void UnitConvertorTest::testLengthConversion()
{
    // If this compiles, compilation works :)
    UnitConvertor<cgs::length, si::length> cmConvertor;

    quantity<si::length, qreal> internalValue(1 * si::meter);
    qreal cm = cmConvertor.fromInternalValue(internalValue);
    QCOMPARE(cm, 100.0);

    internalValue = cmConvertor.toInternalValue(1);
    QCOMPARE(internalValue.value(), 0.01);
}

HR_ADD_TEST(UnitConvertorTest)
