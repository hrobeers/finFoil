/****************************************************************************

 Copyright (c) 2014, Hans Robeers
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

#include "curvefittests.hpp"

#include <boost/filesystem.hpp>

#include "submodules/qtestrunner/qtestrunner.hpp"
#include "hrlib/curvefit/curvefit.hpp"
#include "hrlib/curvefit/vertexio.hpp"

using namespace hrlib;
using namespace std;

void CurveFitTests::testVertexReading()
{
  std::string path = "testdata/profiles/";
  for (auto & p : boost::filesystem::directory_iterator(path))
  {
    std::ifstream ifs;
    ifs.open(p.path().string(), std::ifstream::in);

    std::vector<vertex<2>> curve;
    vertex<2> point;
    while(utf8::read_next_vertex<2>(ifs, point))
      curve.push_back(point);

    if (std::all_of(curve.cbegin(), curve.cend(), [curve](const vertex<2> &p){return p[1] == curve[0][1];}))
      QFAIL(std::string("Failed reading: " + p.path().string()).c_str());
  }

}

void CurveFitTests::testSingle2D()
{
  // Test fitting a bezier curve to 5 points
  vector<vertex<2>> points;
  points.push_back(vertex<2> {0,0});
  points.push_back(vertex<2> {100,100});
  points.push_back(vertex<2> {150,110});
  points.push_back(vertex<2> {200,100});
  points.push_back(vertex<2> {300,0});

  // Test without providing tangents
  auto handles = curve_fit<2>::single(points);
  QVERIFY(qFuzzyCompare(handles[0][0], 146.6666666666667));
  QVERIFY(qFuzzyCompare(handles[0][1], 146.6666666666667));
  QVERIFY(qFuzzyCompare(handles[1][0], 153.3333333333333));
  QVERIFY(qFuzzyCompare(handles[1][1], 146.6666666666667));

  // Test with providing tangents
  handles = curve_fit<2>::single(points, {0,-1}, {0,1});
  QVERIFY(qFuzzyCompare(handles[0][0], 0.0));
  QVERIFY(qFuzzyCompare(handles[0][1], 135.8169954176217));
  QVERIFY(qFuzzyCompare(handles[1][0], 300.0));
  QVERIFY(qFuzzyCompare(handles[1][1], 135.8169954176217));
}

QTR_ADD_TEST(CurveFitTests)
