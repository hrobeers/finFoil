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

#include "foillogic/foilio.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/interprocess/streams/vectorstream.hpp>

#include "hrlib/math/matrix.hpp"
#include "hrlib/curvefit/curvefit.hpp"
#include "hrlib/io/vertexio.hpp"
#include "hrlib/io/pdfio.hpp"
#include "foillogic/profile.hpp"
#include "foillogic/outline.hpp"
#include "patheditor/path.hpp"
#include "patheditor/pathitem.hpp"
#include "patheditor/curvepoint.hpp"
#include "patheditor/line.hpp"
#include "patheditor/controlpoint.hpp"
#include "patheditor/cubicbezier.hpp"

using namespace patheditor;
using namespace foillogic;
using namespace hrlib;

namespace {
  const double OUTLINE_HEIGHT_PX = 260;
  auto comp_x = [](const vertex<2> &v1, const vertex<2> &v2){ return v1[0] < v2[0]; };
  auto comp_y = [](const vertex<2> &v1, const vertex<2> &v2){ return v1[1] < v2[1]; };
}

Profile* foillogic::loadProfileDatStream(std::istream &stream)
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


  //  //
  //  // Fit 4 quadrant sections
  //  //

  //  Path *topProfile = profile->topProfile();
  //  Path *botProfile = profile->botProfile();

  //  auto q1_handles = curve_fit<2>::single(std::vector<vertex<2>>(dat_curve.cbegin(), top), {0,0}, {1,0});
  //  auto q2_handles = curve_fit<2>::single(std::vector<vertex<2>>(top, le), {1,0}, {0,1});
  //  auto q3_handles = curve_fit<2>::single(std::vector<vertex<2>>(le, bot), {0,1}, {-1,0});
  //  auto q4_handles = curve_fit<2>::single(std::vector<vertex<2>>(bot, dat_curve.cend()), {-1,0}, {0,0});

  //  std::shared_ptr<PathItem> q1_item = topItems.last();
  //  q1_item->startPoint()->setRestrictedPos((*top)[0], -(*top)[1]);
  //  q1_item->endPoint()->setRestrictedPos(scale, 0);
  //  q1_item->controlPoints().first()->setRestrictedPos(q1_handles[1][0], -q1_handles[1][1]);
  //  q1_item->controlPoints().last()->setRestrictedPos(q1_handles[0][0], -q1_handles[0][1]);

  //  std::shared_ptr<PathItem> q2_item = topItems.first();
  //  q2_item->startPoint()->setRestrictedPos(0,0);
  //  q2_item->endPoint()->setRestrictedPos((*top)[0], -(*top)[1]);
  //  q2_item->controlPoints().first()->setRestrictedPos(q2_handles[1][0], -q2_handles[1][1]);
  //  q2_item->controlPoints().last()->setRestrictedPos(q2_handles[0][0], -q2_handles[0][1]);

  //  std::shared_ptr<PathItem> q3_item = botItems.first();
  //  q3_item->startPoint()->setRestrictedPos(0,0);
  //  q3_item->endPoint()->setRestrictedPos((*bot)[0], -(*bot)[1]);
  //  q3_item->controlPoints().first()->setRestrictedPos(q3_handles[0][0], -q3_handles[0][1]);
  //  q3_item->controlPoints().last()->setRestrictedPos(q3_handles[1][0], -q3_handles[1][1]);

  //  std::shared_ptr<PathItem> q4_item = botItems.last();
  //  q4_item->startPoint()->setRestrictedPos((*bot)[0], -(*bot)[1]);
  //  q4_item->endPoint()->setRestrictedPos(scale, 0);
  //  q4_item->controlPoints().first()->setRestrictedPos(q4_handles[0][0], -q4_handles[0][1]);
  //  q4_item->controlPoints().last()->setRestrictedPos(q4_handles[1][0], -q4_handles[1][1]);

  return profile.release();
}

namespace {
  void transform(std::vector<double> &values, const mx::Matrix &T)
  {
    assert(values.size()%2==0);
    for (size_t i=0; i<values.size()-1; i+=2)
    {
      mx::Array a = {values[i], values[i+1], 1};
      mx::Vector t = mx::ublas::prod(T, mx::Vector(3,std::move(a)));
      values[i]=t[0];
      values[i+1]=t[1];
    }
  }

  void transform(std::vector<pdf::path_cmd> &values, const mx::Matrix &T,
                 std::pair<hrlib::vertex<2>, hrlib::vertex<2>> *extremes = nullptr)
  {
    hrlib::vertex<2> min, max = {0,0};
    for (pdf::path_cmd &pc : values) {
      transform(pc.vals, T);
      if (extremes)
        for (size_t i=0; i<pc.vals.size(); i++) {
          min[i%2] = std::min(min[i%2], pc.vals[i]);
          max[i%2] = std::max(max[i%2], pc.vals[i]);
        }
    }
    if (extremes)
      *extremes = { std::move(min), std::move(max) };
  }

  template<typename T>
  T* error(std::ostream *err, const char* msg) {
    if (err)
      (*err) << msg << std::endl;
    return nullptr;
  }
}

Outline* foillogic::loadOutlinePdfStream(std::istream &stream, std::ostream *err)
{
  std::unique_ptr<Outline> outline(new Outline());

  // Parse a sequence of path commands (first path encountered)
  std::vector<pdf::path_cmd> path_cmds;
  std::vector<char> bin;
  pdf::path_cmd first;
  while(pdf::read_next_binary(stream, bin))
  {
    std::string line;
    boost::interprocess::basic_ivectorstream<std::vector<char>> bin_stream(bin);
    while(getline_safe(bin_stream, line))
    {
      if (auto pc = pdf::parse_path_line(line)) {
        if (first.cmd!='m' && pc->cmd!='m')
          // continue until first move
          continue;
        if (first.cmd!='m' && pc->cmd=='m')
          // set first move
          first=pc.value();
        else if (first.cmd=='m' && pc->cmd=='m')
        {
          // break on a second move if path long enough
          // otherwise reset move command
          // this ensures alignment lines are skipped
          if (path_cmds.size()>2)
            break;
          path_cmds.clear();
          first=pc.value();
        }
        path_cmds.push_back(std::move(pc.value()));
      }
    }
  }

  // Tranformation matrix for transform to first quadrant
  mx::Vector first_pnt(2, {first.vals[0], first.vals[1]});
  mx::Vector last_pnt(2);
  auto prit = path_cmds.rbegin();
  last_pnt[0] = (*prit).vals[(*prit).vals.size()-2];
  last_pnt[1] = (*prit).vals[(*prit).vals.size()-1];
  if (last_pnt[0]==0 && last_pnt[1]==0) {
    // if path is closed, take last coord of prev section
    prit++;
    last_pnt[0] = (*prit).vals[(*prit).vals.size()-2];
    last_pnt[1] = (*prit).vals[(*prit).vals.size()-1];
    // remove closing command
    path_cmds.pop_back();
  }
  // normalize length
  mx::Vector rotation = last_pnt-first_pnt;
  double length = mx::ublas::norm_2(rotation);
  rotation/=length;
  // construct the transformation matrix
  mx::Array a({rotation[0],rotation[1],first_pnt[0],
               -rotation[1],rotation[0],first_pnt[1],
               0,0,0});
  mx::Matrix Tr = mx::Matrix(3,3,std::move(a));
  std::pair<hrlib::vertex<2>, hrlib::vertex<2>> extr;
  transform(path_cmds, Tr, &extr);
  // rescale
  double minY = extr.second[1];
  double scale = std::abs(OUTLINE_HEIGHT_PX/minY);
  mx::Array s({scale,0,0,
               0,scale,0,
               0,0,0});
  mx::Matrix Ts = mx::Matrix(3,3,std::move(s));
  transform(path_cmds, Ts);

  std::unique_ptr<Path> outline_path(new Path());

  std::shared_ptr<CurvePoint> prev_pnt;
  for (pdf::path_cmd &pc : path_cmds)
  {
    // find the first move command before before building the path
    if (!prev_pnt) {
      if (pc.cmd == 'm' && pc.vals.size()==2)
        prev_pnt = std::make_shared<CurvePoint>(pc.vals[0], -pc.vals[1]);
      continue;
    }

    if (pc.vals.size()<2) continue;
    auto next_pnt = std::make_shared<CurvePoint>(pc.vals[pc.vals.size()-2], -pc.vals[pc.vals.size()-1]);

    // Detect closing (base rotation on this)
    if(next_pnt->x()==0 && next_pnt->y()==0)
      break;

    switch (pc.cmd) {
      case 'l':
        if (pc.vals.size()!=2) return nullptr;
        outline_path->append(std::make_shared<Line>(prev_pnt, next_pnt));
        break;
      case 'c':
        if (pc.vals.size()!=6) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(pc.vals[0], -pc.vals[1]),
                                                           std::make_shared<ControlPoint>(pc.vals[2], -pc.vals[3]),
                                                           next_pnt));
        break;
      case 'v':
        if (pc.vals.size()!=4) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(prev_pnt->x(), prev_pnt->y()),
                                                           std::make_shared<ControlPoint>(pc.vals[0], -pc.vals[1]),
                                                           next_pnt));
        break;
      case 'y':
        if (pc.vals.size()!=4) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(pc.vals[0], -pc.vals[1]),
                                                           std::make_shared<ControlPoint>(next_pnt->x(), next_pnt->y()),
                                                           next_pnt));
        break;
      default:
        return nullptr;
    }
    prev_pnt = next_pnt;
  }

  if (outline_path->pathItems().count()==0)
    return nullptr;
  outline->pSetPath(outline_path.release());
  return outline.release();
}
