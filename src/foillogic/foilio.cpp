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

namespace foillogic {
  // allow parse_profile to be referenced outside
  std::array<std::vector<vertex<2>>,2> parse_profile(std::istream &stream, double scale=1);
}

std::array<std::vector<vertex<2>>,2> foillogic::parse_profile(std::istream &stream, double scale) {
  std::array<std::vector<vertex<2>>,2> dat_curves;
  size_t side=0; // top=0, bottom=1
  double direction=0;
  vertex<2> prev;
  vertex<2> point;
  while(utf8::read_next_vertex_line<2>(stream, point)) {
    if (point[0]<-1 || point[0]>1 ||
        point[1]<-1 || point[1]>1)
      continue;
    point[0] *= scale;
    point[1] *= scale;

    // initialize direction for side
    if (dat_curves[side].size()==1)
      direction = point[0]-prev[0];

    // switch side when x direction reverses
    if (direction!=0 && (point[0]-prev[0])*direction<0)
      if (++side>1)
        break;

    dat_curves[side].push_back(point);
    prev = point;
  }

  // sort curves by increasing x
  std::sort(dat_curves[0].begin(), dat_curves[0].end(), comp_x);
  std::sort(dat_curves[1].begin(), dat_curves[1].end(), comp_x);

  return dat_curves;
}

Profile* foillogic::loadProfileDatStream(std::istream &stream)
{
  const double scale = 300;

  auto dat_curves = parse_profile(stream, scale);

  if (dat_curves[0].size() < 3)
    // a top profile with less than 3 points makes no sense
    return nullptr;

  std::unique_ptr<Profile> profile(new Profile());

  profile->setSymmetry(Profile::Asymmetric);


  //
  // Set Raw points
  //

  std::unique_ptr<Path> topProfile(new Path());
  std::unique_ptr<Path> botProfile(new Path());


  auto le = std::make_shared<CurvePoint>(0,0);
  auto te = std::make_shared<CurvePoint>(scale,0);

  auto prev = le;
  for(auto p : dat_curves[0]) {
    // Skip te and le points
    if (p[1]==0) continue;
    std::shared_ptr<CurvePoint> pnt = std::make_shared<CurvePoint>(p[0], -p[1]);
    topProfile->append(std::make_shared<Line>(prev, pnt));
    prev = pnt;
  }
  topProfile->append(std::make_shared<Line>(prev, te));

  prev = le;
  for(auto p : dat_curves[1]) {
    // Skip te and le points
    if (p[1]==0) continue;
    std::shared_ptr<CurvePoint> pnt = std::make_shared<CurvePoint>(p[0], -p[1]);
    botProfile->append(std::make_shared<Line>(prev, pnt));
    prev = pnt;
  }
  botProfile->append(std::make_shared<Line>(prev, te));

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
  void transform(std::vector<hrlib::vertex<2>> &values, const mx::Matrix &T)
  {
    for (size_t i=0; i<values.size(); ++i)
    {
      mx::Array a = {values[i][0], values[i][1], 1};
      mx::Vector t = mx::ublas::prod(T, mx::Vector(3,std::move(a)));
      values[i] = {t[0], t[1]};
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
          min[0] = std::min(min[0], pc.vals[i][0]);
          max[0] = std::max(max[0], pc.vals[i][0]);
          min[1] = std::min(min[1], pc.vals[i][1]);
          max[1] = std::max(max[1], pc.vals[i][1]);
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

  bool is_closed(const std::vector<pdf::path_cmd> &path_cmds) {
    if (path_cmds.size()<2)
      return false;
    auto first = path_cmds.front();
    auto last = path_cmds.back();
    bool closed = first.vals[first.vals.size()-1][0] == last.vals[last.vals.size()-1][0] &&
                  first.vals[first.vals.size()-1][1] == last.vals[last.vals.size()-1][1];
    return closed;
  }

  double cross(const pdf::path_cmd &p1, const pdf::path_cmd &p2) {
    auto s1 = p1.vals.size();
    auto s2 = p2.vals.size();
    return p1.vals[s1-1][0]*p2.vals[s2-1][1] - p1.vals[s1-1][1]*p2.vals[s2-1][0];
  }

  void reverse(std::vector<pdf::path_cmd> &path) {
    auto it = path.crbegin();
    auto first_it = it++;
    auto prev_it = first_it;
    std::vector<pdf::path_cmd> reversed = {{'m', { prev_it->vals.back() }}};
    while (it != path.crend()) {
      reversed.push_back({prev_it->cmd, std::vector<hrlib::vertex<2>>(prev_it->vals.crbegin()+1, prev_it->vals.crend())});
      reversed.back().vals.push_back(it->vals.back());
      // TODO
      prev_it = it++;
    }
    assert(reversed.size()==path.size());
    path.swap(reversed);
  }
}

Outline* foillogic::loadOutlinePdfStream(std::istream &stream, std::ostream */*err*/)
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
    while(auto pc = pdf::parse_path_line(bin_stream))
    {
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
      if (pc->cmd == 'l' && path_cmds.back().cmd == 'l') {
        // merge line commands that extend each other
        if (pc->vals[0][0] == path_cmds.back().vals[0][0]) {
          path_cmds.back().vals[0][1] = pc->vals[0][1];
          continue;
        }
        if (pc->vals[0][1] == path_cmds.back().vals[0][1]) {
          path_cmds.back().vals[0][0] = pc->vals[0][0];
          continue;
        }
      }
      path_cmds.push_back(std::move(pc.value()));
    }
    // stop reading file when a path is parsed
    if (path_cmds.size()>2)
      break;
  }
  if (path_cmds.size()<2)
    return nullptr;


  //
  // Transform to clockwise path
  //

  pdf::path_cmd prev;
  double c = std::accumulate(path_cmds.cbegin(), path_cmds.cend(), 0,
                             [&prev](double c, const pdf::path_cmd &cmd) {
      if (prev.cmd)
        c += cross(prev, cmd);
      prev = cmd;
      return c;
    });
  if (c > 0)
    reverse(path_cmds);

  //
  // Transform to first quadrant
  //

  // When the path is closed and exactly one line command exists, we can assume it is the base.
  if (is_closed(path_cmds) && std::count_if(path_cmds.cbegin(), path_cmds.cend(), [](pdf::path_cmd c){ return c.cmd == 'l'; }) == 1) {
    // reorder assuming the line command to be the base
    // move everything before l to back
    std::vector<pdf::path_cmd> reordered;
    reordered.reserve(path_cmds.size());
    auto line_it = std::find_if(path_cmds.cbegin(), path_cmds.cend(), [](pdf::path_cmd c){ return c.cmd == 'l'; });
    reordered.insert(reordered.cend(), line_it, path_cmds.cend());
    reordered.insert(reordered.cend(), path_cmds.cbegin(), line_it);
    // erase the useless move command which is now somewhere in the middle
    auto move_it = std::find_if(reordered.cbegin(), reordered.cend(), [](pdf::path_cmd c){ return c.cmd == 'm'; });
    reordered.erase(move_it);
    // turn the first line command into a move command
    reordered[0].cmd = 'm';

    path_cmds.swap(reordered);
  }

  // extract base points for rotation
  first = path_cmds.front();
  mx::Vector first_pnt(2, {first.vals[0][0], first.vals[0][1]});
  mx::Vector last_pnt(2);
  auto prit = path_cmds.rbegin();
  last_pnt[0] = (*prit).vals.back()[0];
  last_pnt[1] = (*prit).vals.back()[1];
  if (is_closed(path_cmds)) {
    // if path is closed, take last coord of prev section
    prit++;
    last_pnt[0] = (*prit).vals.back()[0];
    last_pnt[1] = (*prit).vals.back()[1];
    // remove closing command
    path_cmds.pop_back();
  }

  // normalize length
  mx::Vector rotation = last_pnt-first_pnt;
  double length = mx::ublas::norm_2(rotation);
  rotation/=length;

  // construct the transformation matrix
  mx::Array t({1,0,-first_pnt[0],
               0,1,-first_pnt[1],
               0,0,0});
  mx::Matrix Tt = mx::Matrix(3,3,std::move(t));
  mx::Array r({rotation[0],rotation[1],0,
               -rotation[1],rotation[0],0,
               0,0,0});
  mx::Matrix Tr = mx::Matrix(3,3,std::move(r));
  std::pair<hrlib::vertex<2>, hrlib::vertex<2>> extr;
  transform(path_cmds, Tt);
  transform(path_cmds, Tr, &extr);

  // flip to first quadrant
  hrlib::vertex<2> center = {(extr.first[0]+extr.second[0])/2, (extr.first[1]+extr.second[1])/2};
  mx::Array f({center[0]<0?-1.0:1.0,0,0,
               0,center[1]<0?-1.0:1.0,0,
               0,0,0});
  mx::Matrix Tf = mx::Matrix(3,3,std::move(f));
  transform(path_cmds, Tf, &extr);

  // rescale
  double minY = extr.second[1];
  double scale = std::abs(OUTLINE_HEIGHT_PX/minY);
  mx::Array s({scale,0,0,
               0,scale,0,
               0,0,0});
  mx::Matrix Ts = mx::Matrix(3,3,std::move(s));
  transform(path_cmds, Ts);


  //
  // Construct the outline path
  //

  std::unique_ptr<Path> outline_path(new Path());

  std::shared_ptr<CurvePoint> prev_pnt;
  for (pdf::path_cmd &pc : path_cmds)
  {
    // find the first move command before before building the path
    if (!prev_pnt) {
      if (pc.cmd == 'm' && pc.vals.size()==1)
        prev_pnt = std::make_shared<CurvePoint>(pc.vals[0][0], -pc.vals[0][1]);
      continue;
    }

    if (pc.vals.size()==0) continue;
    auto next_pnt = std::make_shared<CurvePoint>(pc.vals[pc.vals.size()-1][0], -pc.vals[pc.vals.size()-1][1]);

    switch (pc.cmd) {
      case 'l':
        if (pc.vals.size()!=1) return nullptr;
        outline_path->append(std::make_shared<Line>(prev_pnt, next_pnt));
        break;
      case 'c':
        if (pc.vals.size()!=3) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(pc.vals[0][0], -pc.vals[0][1]),
                                                           std::make_shared<ControlPoint>(pc.vals[1][0], -pc.vals[1][1]),
                                                           next_pnt));
        break;
      case 'v':
        if (pc.vals.size()!=2) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(prev_pnt->x(), prev_pnt->y()),
                                                           std::make_shared<ControlPoint>(pc.vals[0][0], -pc.vals[0][1]),
                                                           next_pnt));
        break;
      case 'y':
        if (pc.vals.size()!=2) return nullptr;
        outline_path->append(std::make_shared<CubicBezier>(prev_pnt,
                                                           std::make_shared<ControlPoint>(pc.vals[0][0], -pc.vals[0][1]),
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
