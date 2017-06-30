/****************************************************************************

 Copyright (c) 2017, Hans Robeers
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

#ifndef HRLIB_CURVEFIT_HPP
#define HRLIB_CURVEFIT_HPP

#include <vector>
#include <array>
#include <cmath>
#include <boost/math/special_functions/pow.hpp>
#include <boost/utility/value_init.hpp>

extern "C" {
  #include "curve_fit_nd.h"
}

namespace hrlib {
  template <int Dim>
  using vertex = std::array<double, Dim>;

  template <int Dim>
  struct curve_fit
  {
    static std::vector<vertex<Dim>>
    single(const std::vector<vertex<Dim>> &points,
           vertex<Dim> tan_l = boost::value_initialized<vertex<Dim>>(),
           vertex<Dim> tan_r = boost::value_initialized<vertex<Dim>>())
    {
      auto npts = points.size();

      // Determine lengths between points
      std::vector<double> lengths;
      lengths.push_back(0);
      for (int i=1; i<npts; i++)
        lengths.push_back(sqrt(boost::math::pow<2>(points[i][0]-points[i-1][0]) + boost::math::pow<2>(points[i][1]-points[i-1][1])));

      // Determine tangents based on first and last two points if not provided
      if (std::equal(tan_l.cbegin(), tan_l.cend(), boost::value_initialized<vertex<Dim>>().data().cbegin()))
        tan_l = { (points[0][0]-points[1][0])/lengths[1], (points[0][1]-points[1][1])/lengths[1] };
      if (std::equal(tan_r.cbegin(), tan_r.cend(), boost::value_initialized<vertex<Dim>>().data().cbegin()))
        tan_r = { (points[npts-2][0]-points[npts-1][0])/lengths[npts-1], (points[npts-2][1]-points[npts-1][1])/lengths[npts-1] };

      // Calculate the handles
      std::vector<vertex<Dim>> handles(2);
      double error;
      unsigned int err_idx;
      curve_fit_cubic_to_points_single_db((double*)points.data(), npts, lengths.data(), Dim, 0.0, tan_l.data(), tan_r.data(), handles[0].data(), handles[1].data(), &error, &err_idx);

      return handles;
    }
  };
}

#endif // HRLIB_CURVEFIT_HPP
