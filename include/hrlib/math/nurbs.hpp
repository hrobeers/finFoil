/****************************************************************************

 Copyright (c) 2018, Hans Robeers
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

#ifndef HRLIB_NURBS_HPP
#define HRLIB_NURBS_HPP

#include <array>
#include <deque>

namespace nurbs
{
  template<typename Tpnt>
  struct pnt_access {
    static constexpr auto x(const Tpnt& p) { return p.x(); }
    static constexpr auto y(const Tpnt& p) { return p.y(); }
  };
  template<typename Tpath, typename Tpnt = typename Tpath::value_type, typename Tpa = pnt_access<Tpnt>>
  std::array<std::deque<Tpnt>,2> casteljau(const Tpath& p, double t) {
    if (p.size()==0)
      return std::array<std::deque<Tpnt>,2>();

    std::deque<Tpnt> sub;
    auto it = p.cbegin();
    Tpnt prev = *it;
    while (p.cend() != ++it) {
      sub.push_back(Tpnt {
                          (Tpa::x(*it) - Tpa::x(prev))*t + Tpa::x(prev),
                          (Tpa::y(*it) - Tpa::y(prev))*t + Tpa::y(prev)
        });
      prev = *it;
    }

    auto retval = casteljau(sub, t);
    retval.front().push_front(p.front());
    retval.back().push_back(p.back());

    return retval;
  }
}

#endif // HRLIB_NURBS_HPP
