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

#include "foillogic/profileloader.hpp"

#include <boost/algorithm/string.hpp>
#include "hrlib/curvefit/curvefit.hpp"
#include "hrlib/curvefit/vertexio.hpp"
#include "foillogic/profile.hpp"
#include "patheditor/path.hpp"
#include "patheditor/pathitem.hpp"
#include "patheditor/curvepoint.hpp"
#include "patheditor/line.hpp"

using namespace patheditor;
using namespace foillogic;
using namespace hrlib;

namespace {
  constexpr auto comp_x = [](const vertex<2> &v1, const vertex<2> &v2){ return v1[0] < v2[0]; };
  constexpr auto comp_y = [](const vertex<2> &v1, const vertex<2> &v2){ return v1[1] < v2[1]; };
}

Profile* ProfileLoader::loadDatStream(std::istream &stream)
{
  const double scale = 300;

  std::vector<vertex<2>> dat_curve;
  vertex<2> point;
  while(utf8::read_next_vertex<2>(stream, point))
  {
    point[0] *= scale;
    point[1] *= scale;
    dat_curve.push_back(point);
  }

  auto le = std::min_element(dat_curve.cbegin(), dat_curve.cend(), comp_x);
//  auto top = std::max_element(dat_curve.cbegin(), dat_curve.cend(), comp_y);
//  auto bot = std::min_element(dat_curve.cbegin(), dat_curve.cend(), comp_y);
//  auto te = std::max_element(dat_curve.cbegin(), bot, comp_x);

  std::unique_ptr<Profile> profile(new Profile());

  profile->setSymmetry(Profile::Asymmetric);


  //
  // Set Raw points
  //

  std::unique_ptr<Path> topProfile(new Path());
  std::unique_ptr<Path> botProfile(new Path());

  std::shared_ptr<CurvePoint> prev;
  std::shared_ptr<CurvePoint> le_pnt, te_pnt;
  for(auto it = le; it != dat_curve.cbegin(); --it)
    {
      std::shared_ptr<CurvePoint> pnt = std::make_shared<CurvePoint>((*it)[0], -(*it)[1]);
      if (prev)
        topProfile->append(std::make_shared<Line>(prev, pnt));
      else
        le_pnt = pnt;
      prev.swap(pnt);
    }

  te_pnt = prev;
  prev = le_pnt;
  for(auto it = le+1; it != dat_curve.cend(); ++it)
    {
      std::shared_ptr<CurvePoint> pnt = std::make_shared<CurvePoint>((*it)[0], -(*it)[1]);
      if (prev)
        {
          botProfile->append(std::make_shared<Line>(prev, pnt));
        }
      prev.swap(pnt);
    }

  profile->pSetTopProfile(topProfile.release());
  profile->pSetBotProfile(botProfile.release());

  return profile.release();
}
