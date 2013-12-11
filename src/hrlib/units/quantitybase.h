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

#ifndef HRLIB_QUANTITYBASE_H
#define HRLIB_QUANTITYBASE_H

#include "iquantity.h"
#include "boost/units/quantity.hpp"
#include "hrlib/units/unitconvertor.h"
#include <map>

namespace hrlib {
namespace units {

    template<typename UnitEnum, class InternalUnit, typename NumericType = qreal>
    class QuantityBase : public IQuantity
    {
    private:
        UnitEnum _unit;
        boost::units::quantity<InternalUnit, NumericType> _internalValue;

        static std::map<UnitEnum, const UnitConvertorBase<InternalUnit, NumericType>*>& convertorMap()
        {
            static std::map<UnitEnum, const UnitConvertorBase<InternalUnit, NumericType>*> convertorMap;
            return convertorMap;
        }

    public:
        static const UnitConvertorBase<InternalUnit, NumericType>* convertor(UnitEnum unit)
        {
            return convertorMap().at(unit);
        }

        explicit QuantityBase(boost::units::quantity<InternalUnit, NumericType> internalValue,
                              UnitEnum unit) :
            _internalValue(internalValue), _unit(unit)
        {
        }

        void setUnit(UnitEnum unitEnum)
        {
            _unit = unitEnum;
        }

        virtual qreal value() const
        {
            return convertor(_unit)->fromInternalValue(_internalValue);
        }

        virtual void setValue(qreal value)
        {
            _internalValue = convertor(_unit)->toInternalValue(value);
        }

        virtual QString unitSymbol() const
        {
            return convertor(_unit)->unitSymbol();
        }

        inline boost::units::quantity<InternalUnit, NumericType> internalValue() const
            { return _internalValue; }
        inline void setInternalValue(boost::units::quantity<InternalUnit, NumericType> value)
            { _internalValue = value; }

        virtual ~QuantityBase() {}

        class insertConvertor
        {
        public:
            explicit insertConvertor(UnitEnum convEnum, const UnitConvertorBase<InternalUnit, NumericType>* convertor)
            {
                convertorMap()[convEnum] = convertor;
            }
        };
    };

} // namespace units
} // namespace hrlib

#endif // HRLIB_QUANTITYBASE_H
