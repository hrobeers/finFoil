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

#ifndef FOILLOGIC_SAMPLERS_HPP
#define FOILLOGIC_SAMPLERS_HPP

#include "foillogic/fwd/foillogicfwd.hpp"
#include "finfoil/patheditor/fwd/patheditorfwd.hpp"

#include <iterator>
#include <QPointF>
#include <vector>
#include <experimental/optional>
#include <functional>


namespace foillogic
{
    class ProfileSampler
    {
    public:
        ProfileSampler(foillogic::Profile *profile, qreal thick, qreal minThick, int resolution);
        ~ProfileSampler() {}

        void resetTop(qreal thicknessFactor);
        std::experimental::optional<QPointF> nextTop();

        void resetBot(qreal thicknessFactor);
        std::experimental::optional<QPointF> nextBot();

        qreal shift() { return _profileShift; }

    private:
        foillogic::Profile* _profile;
        qreal _fThickTop, _fThickBot;
        qreal _thick, _minThick, _profileShift;
        qreal _topMinThick, _botMinThick;
        int _idxTop, _idxBot, _resolution;

        std::vector<QPointF> _normalizedDiscreteTop;
        std::vector<QPointF> _normalizedDiscreteBot;

        qreal scaleY(qreal normalizedZ, qreal thicknessFactor, qreal minimumThickness);
    };

    class FeatureSampler
    {
      std::vector<qreal> _featureSamples;

    public:
      FeatureSampler();
      void addFeatureSamples(const patheditor::IPath *path, std::function<qreal(QPointF)> getter, size_t sample_rate);
      void addUniformSamples(double min, double max, size_t cnt);
      std::vector<qreal> sampleAt(size_t resolution);
    };

  std::vector<qreal> sampleThickess(const patheditor::IPath *thicknessProfile, const std::vector<qreal> &sectionHeightArray);
}

#endif // FOILLOGIC_SAMPLERS_HPP
