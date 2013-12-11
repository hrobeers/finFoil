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

#ifndef HRLIB_SYSTEMS_H
#define HRLIB_SYSTEMS_H

#include "boost/units/make_system.hpp"
#include "boost/units/base_units/imperial/foot.hpp"
#include "boost/units/base_units/imperial/inch.hpp"
#include "boost/units/physical_dimensions/area.hpp"

namespace hrlib {
namespace units {

    namespace ft {
        typedef boost::units::make_system<
            boost::units::imperial::foot_base_unit
            >::type ft_system;

        typedef boost::units::unit<boost::units::length_dimension, ft_system> length;
        typedef boost::units::unit<boost::units::area_dimension, ft_system> area;
    }
    namespace inch {
        typedef boost::units::make_system<
            boost::units::imperial::inch_base_unit
            >::type in_system;

    typedef boost::units::unit<boost::units::length_dimension, in_system> length;
    typedef boost::units::unit<boost::units::area_dimension, in_system> area;
    }


} // namespace units
} // namespace hrlib

#endif // HRLIB_SYSTEMS_H
