/****************************************************************************

 Copyright (c) 2015, Hans Robeers
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

#include "foillogic/samplers.hpp"

#include <boost/math/tools/roots.hpp>
#include "foillogic/profile.hpp"
#include "patheditor/path.hpp"

using namespace foillogic;
using namespace patheditor;
using namespace boost::math;

namespace {
  typedef boost::math::policies::policy<boost::math::policies::evaluation_error<boost::math::policies::ignore_error>> ignore_eval_err;
  const std::function<bool(qreal,qreal)> stop_condition = [](qreal a, qreal b){return std::abs(a-b) < 0.0001;};
}

FeatureSampler::FeatureSampler() { _featureSamples.push_back(0); }

void FeatureSampler::addFeatureSamples(const IPath *path, std::function<qreal(QPointF)> getter, size_t sample_rate)
{
  for (size_t i=0; i<sample_rate; i++)
    _featureSamples.push_back(
      // start at i+1 to avoid 0 duplication
      // divide by sample_rate+2 to avoid 1 duplication
      getter(path->pointAtPercent(qreal(i+1)/qreal(sample_rate+2)))
          );
}

void FeatureSampler::addUniformSamples(double min, double max, size_t cnt)
{
  double increment = (max-min)/double(cnt);
  for (double v=min; v<max; v+=increment)
    _featureSamples.push_back(v);
}

std::vector<qreal> FeatureSampler::sampleAt(size_t resolution)
{
  // Sort and deduplicate the feature samples
  std::sort(_featureSamples.begin(), _featureSamples.end());
  _featureSamples.erase(std::unique(_featureSamples.begin(), _featureSamples.end()), _featureSamples.end());
  // Subsample the feature samples at resolution
  std::vector<qreal> sampled;
  for (size_t i=0; i<resolution;i++)
    {
      size_t idx = i*_featureSamples.size()/resolution;
      sampled.push_back(_featureSamples[idx]);
    }
  return sampled;
}

std::vector<qreal> foillogic::sampleThickess(const IPath *thicknessProfile, const std::vector<qreal> &sectionHeightArray)
{
  std::vector<qreal> sampled(sectionHeightArray.size());
  size_t i = 0;
  for (qreal height : sectionHeightArray)
    {
      auto f_thickness = [=](qreal perc) -> qreal {
          return thicknessProfile->pointAtPercent(perc).x()-height;
        };

      boost::uintmax_t max_it = 50;
      auto sol = tools::bisect(f_thickness, 0.0, 1.0, stop_condition, max_it, ignore_eval_err());
      sampled[i++] = thicknessProfile->pointAtPercent(sol.first).y();
    }
  return sampled;
}
