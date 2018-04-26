/****************************************************************************

 Copyright (c) 2013, Hans Robeers
 All rights reserved.

 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef CONTOURCALCULATOR_HPP
#define CONTOURCALCULATOR_HPP

#include <QRunnable>
#include <QPointF>

#include <boost/math/tools/roots.hpp>
#include "patheditor/pathfunctors.hpp"
#include "hrlib/math/spline.hpp"
#include "patheditor/path.hpp"
#include "foillogic/samplers.hpp"

using namespace patheditor;
using namespace boost::math;
using namespace boost::math::tools;

namespace foillogic
{
    inline bool isInRange(qreal x, qreal a, qreal b)
    {
        if (x >= a && x <= b)
            return true;
        if (x <= a && x >= b)
            return true;
        return false;
    }

    inline bool includePoint(QPointF *prevPoint, QPointF *curPoint)
    {
        if (!prevPoint)
            return true;

        QPointF diff = (*curPoint - *prevPoint);

        if (qAbs(diff.x()/diff.y()) > 1)
            return true;

        return false;
    }

    template<typename Target>
    class ContourCalculator : public QRunnable
    {
        enum SplineFunction { bSpline, overhauser };

        Target *_result;
        qreal _percContourHeight;
        const patheditor::IPath* _outline;
        const patheditor::IPath* _thickness;
        const patheditor::IPath* _profile;

        size_t _sectionCount;
        size_t _resolution;
        qreal _tTol;

    public:
        explicit ContourCalculator(Target* result, qreal percContourHeight,
                                   const IPath* outline, const IPath* thickness, const IPath* profile,
                                   size_t sectionCount = 512, size_t resolution = 512) :
          _result(result), _percContourHeight(percContourHeight),
          _outline(outline), _thickness(thickness), _profile(profile),
          _sectionCount(sectionCount), _resolution(resolution), _tTol(0.00001)
        {}

        virtual void run()
        {
            qreal t_profileTop;
            qreal y_profileTop;

            y_profileTop = _profile->maxY(&t_profileTop);
            qreal profileLength = _profile->pointAtPercent(1).x();


            //
            // discretise and normalise the thickness profile in different cross sections
            //

            const double mult = 2;
            FeatureSampler featureSampler;
            featureSampler.addFeatureSamples(_outline, [](QPointF p){return p.y();}, _sectionCount);
            featureSampler.addFeatureSamples(_thickness, [](QPointF p){return p.x();}, _sectionCount);
            featureSampler.addUniformSamples(0, _thickness->pointAtPercent(1).x(), _sectionCount*mult);
            std::vector<qreal> sectionHeightArray = featureSampler.sampleAt(_sectionCount);
            std::vector<qreal> thicknessArray = sampleThickess(_thickness, sectionHeightArray);
            // normalize
            qreal height = _thickness->pointAtPercent(1).x();
            qreal maxThickness = qMax(qAbs(_thickness->minY()), qAbs(_thickness->pointAtPercent(0).y()));
            for (qreal &h : sectionHeightArray) h/=height;
            for (qreal &t : thicknessArray) t/=maxThickness;


            //
            // create the functors
            //

            f_diffTol<qreal> tTolerance(_tTol);
            f_ValueAtPercentPath<Y> yOutline(_outline);
            f_ValueAtPercentPath<Y> yProfile(_profile);


            //
            // find the top of the outline
            //

            qreal t_top = 0.5; // start value
            qreal y_top = _outline->maxY(&t_top);


            //
            // calculate the contour points
            //

            std::vector<QPointF*> leadingEdgePnts;
            std::vector<QPointF*> trailingEdgePnts;

            QPointF outlineLeadingEdge, outlineTrailingEdge;
            qreal leadingEdgePerc, trailingEdgePerc;
            for (size_t i=0; i<_sectionCount; i++)
            {
                qreal thicknessOffsetPercent = _percContourHeight / thicknessArray[i];

                qreal profileOffset = thicknessOffsetPercent * y_profileTop;
                yProfile.setOffset(profileOffset);

                // Set t_profileTop to t_min of the profile for correct calculation of the Negative profile
                qreal y_profileBot = 0;
                if (_percContourHeight < 0)
                {
                    y_profileBot = _profile->minY(&t_profileTop);
                }

                if (!isInRange(profileOffset, y_profileBot, y_profileTop))
                {
                    leadingEdgePnts.push_back(0);
                    trailingEdgePnts.push_back(0);
                    continue;
                }

                try
                {
                    yOutline.setOffset(sectionHeightArray[i] * y_top);

                    qreal t_outlineLeadingEdge = bisect(yOutline, 0.0, t_top, tTolerance).first;
                    qreal t_outlineTrailingEdge = bisect(yOutline, t_top, 1.0, tTolerance).first;

                    outlineLeadingEdge = _outline->pointAtPercent(t_outlineLeadingEdge);
                    outlineTrailingEdge = _outline->pointAtPercent(t_outlineTrailingEdge);

                    qreal t_profileLE = bisect(yProfile, 0.0, t_profileTop, tTolerance).first;
                    qreal t_profileTE = bisect(yProfile, t_profileTop, 1.0, tTolerance).first;

                    leadingEdgePerc = _profile->pointAtPercent(t_profileLE).x() / profileLength;
                    trailingEdgePerc = _profile->pointAtPercent(t_profileTE).x() / profileLength;
                }
                catch (evaluation_error /*unused*/)
                {
                    // no result when bisect fails
                    leadingEdgePnts.push_back(nullptr);
                    trailingEdgePnts.push_back(nullptr);
                    continue;
                }

                qreal xLE = outlineLeadingEdge.x();
                qreal xTE = outlineTrailingEdge.x();

                std::unique_ptr<QPointF> leadingEdgePnt(new QPointF(xLE +(leadingEdgePerc * (xTE - xLE)), outlineLeadingEdge.y()));
                std::unique_ptr<QPointF> trailingEdgePnt(new QPointF(xLE +(trailingEdgePerc * (xTE - xLE)), outlineTrailingEdge.y()));

                if (i%5 == 0)
                {
                    // Make sure that at least one in 5 points is added for spline evaluation
                    leadingEdgePnts.push_back(leadingEdgePnt.release());
                    trailingEdgePnts.push_back(trailingEdgePnt.release());
                }
                else if (includePoint(leadingEdgePnts.back(), leadingEdgePnt.get()) ||
                         includePoint(trailingEdgePnts.back(), trailingEdgePnt.get()))
                {
                    // Only add point for spline evaluation when abs(x/y) > 1 (close to the tip)
                    leadingEdgePnts.push_back(leadingEdgePnt.release());
                    trailingEdgePnts.push_back(trailingEdgePnt.release());
                }
            }

            int pointCount = leadingEdgePnts.size();

            int firstIndex = 0;
            for (int i=0; i<pointCount; i++)
            {
                if (leadingEdgePnts[i] != 0)
                {
                    if (i == 0 || leadingEdgePnts[i-1] == 0) // first or nullptr
                        firstIndex = i;

                    if (i == pointCount-1 || leadingEdgePnts[i+1] == 0)
                    {
                        //createLinePath(leadingEdgePnts.data(), trailingEdgePnts.data(), firstIndex, i);
                        createSplinePath(leadingEdgePnts.data(), trailingEdgePnts.data(), firstIndex, i, bSpline);
                    }
                }
            }

            // cleanup
            foreach (QPointF *point, leadingEdgePnts) if (point) delete point;
            foreach (QPointF *point, trailingEdgePnts) if (point) delete point;
        }

        virtual ~ContourCalculator() {}

    private:
        void smoothLaplacian(QPointF* pnts[], int firstIndex, int lastIndex, int it_cnt=1)
        {
            for(; it_cnt>0; it_cnt--)
            for (int i = firstIndex+1; i <= lastIndex-1; i++)
            {
              pnts[i]->setX((pnts[i-1]->x()+pnts[i+1]->x())/2);
              pnts[i]->setY((pnts[i-1]->y()+pnts[i+1]->y())/2);
            }
        }
        void createLinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex)
        {
            _result->moveTo(*leadingEdgePnts[firstIndex]);

            for (int i = firstIndex + 1; i <= lastIndex; i++)
                _result->lineTo(*leadingEdgePnts[i]);

            for (int i = lastIndex; i >= firstIndex; i--)
                _result->lineTo(*trailingEdgePnts[i]);

            if (firstIndex != 0)
                _result->lineTo(*leadingEdgePnts[firstIndex]);
        }
        void createSplinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex, SplineFunction splineFunction)
        {
            if (firstIndex >= lastIndex)
            {
                _result->moveTo(*leadingEdgePnts[firstIndex]);
                return;
            }

            bool closing = firstIndex != 0;
            std::vector<qreal> points_x;
            std::vector<qreal> points_y;

            if (closing)
            {
                points_x.push_back(trailingEdgePnts[firstIndex]->x());
                points_y.push_back(trailingEdgePnts[firstIndex]->y());
            }
            for (int i = firstIndex; i <= lastIndex; i++)
            {
                points_x.push_back(leadingEdgePnts[i]->x());
                points_y.push_back(leadingEdgePnts[i]->y());
            }
            for (int i = lastIndex; i >= firstIndex; i--)
            {
                points_x.push_back(trailingEdgePnts[i]->x());
                points_y.push_back(trailingEdgePnts[i]->y());
            }
            if (closing)
            {
                points_x.push_back(points_x[1]);
                points_y.push_back(points_y[1]);

                points_x.push_back(points_x[2]);
                points_y.push_back(points_y[2]);
            }

            int pointCount = points_x.size();

            qreal t_val = 0;
            qreal t_valStep;
            if (closing)
            {
                t_val = 1;
                t_valStep = (pointCount-3) / qreal(_resolution);
            }
            else
            {
                t_valStep = (pointCount-1) / qreal(_resolution);
            }

            qreal x, y;
            switch(splineFunction)
            {
            case bSpline:
                x = hrlib::spline_b_val(pointCount, points_x.data(), t_val);
                y = hrlib::spline_b_val(pointCount, points_y.data(), t_val);
                _result->moveTo(x, y);
                for (size_t i = 1; i <= _resolution; i++)
                {
                    t_val += t_valStep;
                    x = hrlib::spline_b_val(pointCount, points_x.data(), t_val);
                    y = hrlib::spline_b_val(pointCount, points_y.data(), t_val);
                    _result->lineTo(x, y);
                }
                break;

            case overhauser:
                std::vector<qreal> t_data(pointCount);
                for (size_t i = 0; i < t_data.size(); i++)
                    t_data[i] = i;

                x = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_x.data(), t_val);
                y = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_y.data(), t_val);
                _result->moveTo(x, y);
                for (size_t i = 1; i <= _resolution; i++)
                {
                    t_val += t_valStep;
                    x = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_x.data(), t_val);
                    y = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_y.data(), t_val);
                    _result->lineTo(x, y);
                }
                break;
            }
        }
    };

}

#endif // CONTOURCALCULATOR_HPP
