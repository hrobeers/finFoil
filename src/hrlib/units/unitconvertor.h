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

#ifndef HRLIB_UNITTYPE_H
#define HRLIB_UNITTYPE_H

#include <QString>
#include "boost/units/quantity.hpp"
#include "boost/mpl/string.hpp"

namespace hrlib {
namespace units {

    template<class InternalUnit, typename NumericType = qreal>
    class UnitConvertorBase
    {
    public:
        virtual NumericType fromInternalValue(boost::units::quantity<InternalUnit, NumericType> internalValue) const = 0;
        virtual boost::units::quantity<InternalUnit, NumericType> toInternalValue(NumericType value) const = 0;
        virtual QString unitSymbol() const = 0;

        virtual ~UnitConvertorBase() {}
    };

    template<class Unit, class InternalUnit,
             typename UnitSymbol, typename NumericType = qreal>
    class UnitConvertor : public UnitConvertorBase<InternalUnit>
    {
    public:
        virtual NumericType fromInternalValue(boost::units::quantity<InternalUnit, NumericType> internalValue) const
        {
            boost::units::quantity<Unit, NumericType> converted(internalValue);
            return converted.value();
        }

        virtual boost::units::quantity<InternalUnit, NumericType> toInternalValue(NumericType value) const
        {
            boost::units::quantity<InternalUnit, NumericType> internalValue(value * Unit());
            return internalValue;
        }

        virtual QString unitSymbol() const
        {
            return QString(boost::mpl::c_str<UnitSymbol>::value);
        }

        virtual ~UnitConvertor() {}
    };

} // namespace units
} // namespace hrlib

#endif // HRLIB_UNITTYPE_H
