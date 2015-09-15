/****************************************************************************

 Copyright (c) 2013, Hans Robeers
 All rights reserved.

 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

****************************************************************************/

#include "contourcalculator.h"

#include <QVarLengthArray>
#include <QPainterPath>
#include <boost/math/tools/roots.hpp>
#include "pathfunctors.hpp"
#include "math/spline.hpp"
#include "foil.h"
#include "profile.h"
#include "thicknessprofile.h"
#include "outline.h"
#include "path.h"

#ifdef QT_DEBUG
    #define INITCNT 128
    #define LOW_RES 20
    #define HI_RES 50
#else
    #define INITCNT 1024
    #define LOW_RES 200
    #define HI_RES 500
#endif

using namespace foillogic;
using namespace patheditor;
using namespace boost::math;
using namespace boost::math::tools;

namespace
{
    bool isInRange(qreal x, qreal a, qreal b)
    {
        if (x > a && x < b)
            return true;
        if (x < a && x > b)
            return true;
        return false;
    }
}

ContourCalculator::ContourCalculator(qreal percContourHeight, Foil *foil, QPainterPath *result, Side::e side, bool fast) :
    _side(side), _outline(foil->outline()->path()), _thickness(foil->thicknessProfile()->topProfile()), _percContourHeight(percContourHeight),
    _result(result), _sectionCount(INITCNT / 8), _resolution(LOW_RES), _tTol(0.0015)
{
    switch (_side) {
    case Side::Bottom:
        _profile = foil->profile()->botProfile();
        break;
    default:
        _profile = foil->profile()->topProfile();
        break;
    }

    if (!fast)
    {
        _sectionCount = INITCNT / 2;
        _resolution = HI_RES;
        _tTol = 0.0001;
    }
}

static bool includePoint(QPointF *prevPoint, QPointF *curPoint)
{
    if (!prevPoint)
        return true;

    QPointF diff = (*curPoint - *prevPoint);

    if (qAbs(diff.x()/diff.y()) > 1)
        return true;

    return false;
}

void ContourCalculator::run()
{
    qreal t_profileTop;
    qreal y_profileTop;

    switch (_side) {
    case Side::Bottom:
        y_profileTop = _profile->maxY(&t_profileTop);
        break;
    default:
        y_profileTop = _profile->minY(&t_profileTop);
        break;
    }

    qreal profileLength = _profile->pointAtPercent(1).x();


    //
    // discretise and normalise the thickness profile in different cross sections
    //

    QVarLengthArray<qreal, INITCNT> sectionHeightArray(_sectionCount);
    QVarLengthArray<qreal, INITCNT> thicknessArray(_sectionCount);
    sampleThickess(_thickness, _sectionCount, sectionHeightArray.data(), thicknessArray.data(), true);


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
    qreal y_top = _outline->minY(&t_top);


    //
    // calculate the contour points
    //

    QVarLengthArray<QPointF*, INITCNT> leadingEdgePnts;
    QVarLengthArray<QPointF*, INITCNT> trailingEdgePnts;

    QPointF outlineLeadingEdge, outlineTrailingEdge;
    qreal leadingEdgePerc, trailingEdgePerc;
    for (int i=0; i<_sectionCount; i++)
    {
        qreal thicknessOffsetPercent = _percContourHeight / thicknessArray[i];

        qreal profileOffset = thicknessOffsetPercent * y_profileTop;
        yProfile.setOffset(profileOffset);

        // Set t_profileTop to t_min of the profile for correct calculation of the Negative profile
        qreal y_profileBot = 0;
        if (_percContourHeight < 0)
        {
            if (_side == Side::Bottom)
                y_profileBot = _profile->minY(&t_profileTop);
            else
                y_profileBot = _profile->maxY(&t_profileTop);
        }

        if (!isInRange(profileOffset, y_profileBot, y_profileTop))
        {
            leadingEdgePnts.append(0);
            trailingEdgePnts.append(0);
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
            leadingEdgePnts.append(0);
            trailingEdgePnts.append(0);
            continue;
        }

        qreal xLE = outlineLeadingEdge.x();
        qreal xTE = outlineTrailingEdge.x();

        std::unique_ptr<QPointF> leadingEdgePnt(new QPointF(xLE +(leadingEdgePerc * (xTE - xLE)), outlineLeadingEdge.y()));
        std::unique_ptr<QPointF> trailingEdgePnt(new QPointF(xLE +(trailingEdgePerc * (xTE - xLE)), outlineTrailingEdge.y()));

        if (i%5 == 0)
        {
            // Make sure that at least one in 5 points is added for spline evaluation
            leadingEdgePnts.append(leadingEdgePnt.release());
            trailingEdgePnts.append(trailingEdgePnt.release());
        }
        else if (includePoint(leadingEdgePnts.last(), leadingEdgePnt.get()) ||
                 includePoint(trailingEdgePnts.last(), trailingEdgePnt.get()))
        {
            // Only add point for spline evaluation when abs(x/y) > 1 (close to the tip)
            leadingEdgePnts.append(leadingEdgePnt.release());
            trailingEdgePnts.append(trailingEdgePnt.release());
        }
    }

    int pointCount = leadingEdgePnts.count();

    int firstIndex = 0;
    for (int i=0; i<pointCount; i++)
    {
        if (leadingEdgePnts[i] != 0)
        {
            if (i == 0 || leadingEdgePnts[i-1] == 0) // first or nullptr
                firstIndex = i;

            if (i == pointCount-1 || leadingEdgePnts[i+1] == 0)
            {
//                createLinePath(leadingEdgePnts.data(), trailingEdgePnts.data(), firstIndex, i);
                createSplinePath(leadingEdgePnts.data(), trailingEdgePnts.data(), firstIndex, i, bSpline);
            }
        }
    }

    // cleanup
    foreach (QPointF *point, leadingEdgePnts) if (point) delete point;
    foreach (QPointF *point, trailingEdgePnts) if (point) delete point;
}

void ContourCalculator::sampleThickess(IPath *thicknessProfile, int sectionCount, qreal sectionHeightArray[], qreal thicknessArray[], bool normalize)
{
    qreal increment = qreal(1) / sectionCount;
    qreal t = 0;
    qreal height = thicknessProfile->pointAtPercent(1).x();
    qreal maxThickness = qMin(thicknessProfile->minY(), thicknessProfile->pointAtPercent(0).y());
    for (int i=0; i<sectionCount; i++)
    {
        QPointF p = thicknessProfile->pointAtPercent(t);

        sectionHeightArray[i] = p.x();
        thicknessArray[i] = p.y();
        if (normalize)
        {
            sectionHeightArray[i] /= height;
            thicknessArray[i] /= maxThickness;
        }
        t += increment;
    }
}

void ContourCalculator::createLinePath(QPointF *leadingEdgePnts[], QPointF *trailingEdgePnts[], int firstIndex, int lastIndex)
{
    _result->moveTo(*leadingEdgePnts[firstIndex]);

    for (int i = firstIndex + 1; i <= lastIndex; i++)
        _result->lineTo(*leadingEdgePnts[i]);

    for (int i = lastIndex; i >= firstIndex; i--)
        _result->lineTo(*trailingEdgePnts[i]);

    if (firstIndex != 0)
        _result->lineTo(*leadingEdgePnts[firstIndex]);
}

void ContourCalculator::createSplinePath(QPointF *leadingEdgePnts[], QPointF *trailingEdgePnts[],
                                         int firstIndex, int lastIndex, SplineFunction splineFunction)
{
    if (firstIndex >= lastIndex)
        return;

    bool closing = firstIndex != 0;
    QVarLengthArray<qreal, INITCNT> points_x;
    QVarLengthArray<qreal, INITCNT> points_y;

    if (closing)
    {
        points_x.append(trailingEdgePnts[firstIndex]->x());
        points_y.append(trailingEdgePnts[firstIndex]->y());
    }
    for (int i = firstIndex; i <= lastIndex; i++)
    {
        points_x.append(leadingEdgePnts[i]->x());
        points_y.append(leadingEdgePnts[i]->y());
    }
    for (int i = lastIndex; i >= firstIndex; i--)
    {
        points_x.append(trailingEdgePnts[i]->x());
        points_y.append(trailingEdgePnts[i]->y());
    }
    if (closing)
    {
        points_x.append(points_x[1]);
        points_y.append(points_y[1]);

        points_x.append(points_x[2]);
        points_y.append(points_y[2]);
    }

    int pointCount = points_x.count();

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
        for (int i = 1; i <= _resolution; i++)
        {
            t_val += t_valStep;
            x = hrlib::spline_b_val(pointCount, points_x.data(), t_val);
            y = hrlib::spline_b_val(pointCount, points_y.data(), t_val);
            _result->lineTo(x, y);
        }
        break;

    case overhauser:
        QVarLengthArray<qreal, INITCNT> t_data(pointCount);
        for (int i = 0; i < t_data.count(); i++)
            t_data[i] = i;

        x = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_x.data(), t_val);
        y = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_y.data(), t_val);
        _result->moveTo(x, y);
        for (int i = 1; i <= _resolution; i++)
        {
            t_val += t_valStep;
            x = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_x.data(), t_val);
            y = hrlib::spline_overhauser_uni_val(pointCount, t_data.data(), points_y.data(), t_val);
            _result->lineTo(x, y);
        }
        break;
    }
}
