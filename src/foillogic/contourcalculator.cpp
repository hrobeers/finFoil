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
#include "hrlib/math/spline.hpp"
#include "foil.h"
#include "profile.h"
#include "thicknessprofile.h"
#include "outline.h"

#define INITCNT 256

using namespace foillogic;
using namespace patheditor;
using namespace boost::math;
using namespace boost::math::tools;

ContourCalculator::ContourCalculator(qreal percContourHeight, Foil *foil, QPainterPath *result, Side::e side, bool fast) :
    _side(side), _outline(foil->outline()->path()), _thickness(foil->thickness()->topProfile()), _percContourHeight(percContourHeight),
    _result(result), _sectionCount(INITCNT / 2), _resolution(200), _tTol(0.0015)
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
        _sectionCount = INITCNT * 2;
        _resolution = 500;
        _tTol = 0.0001;
    }
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
    sampleThickess(sectionHeightArray.data(), thicknessArray.data());


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

    for (int i=0; i<_sectionCount; i++)
    {
        qreal thicknessOffsetPercent = _percContourHeight / thicknessArray[i];
        // if Offset > 1, no intersection exists
        if (thicknessOffsetPercent > 1)
        {
            leadingEdgePnts.append(0);
            trailingEdgePnts.append(0);
            continue;
        }
        else if (thicknessOffsetPercent < 0)
        {
            thicknessOffsetPercent = 0;
        }

        yOutline.setOffset(sectionHeightArray[i] * y_top);

        qreal t_outlineLeadingEdge = bisect(yOutline, 0.0, t_top, tTolerance).first;
        qreal t_outlineTrailingEdge = bisect(yOutline, t_top, 1.0, tTolerance).first;

        QPointF outlineLeadingEdge = _outline->pointAtPercent(t_outlineLeadingEdge);
        QPointF outlineTrailingEdge = _outline->pointAtPercent(t_outlineTrailingEdge);

        // Set t_profileTop to t_min of the profile for correct calculation of the Negative profile
        if (_percContourHeight < 0)
        {
            if (_side == Side::Bottom)
                _profile->minY(&t_profileTop);
            else
                _profile->maxY(&t_profileTop);
        }

        qreal profileOffset = thicknessOffsetPercent * y_profileTop;
        yProfile.setOffset(profileOffset);

        qreal t_profileLE = bisect(yProfile, 0.0, t_profileTop, tTolerance).first;
        qreal t_profileTE = bisect(yProfile, t_profileTop, 1.0, tTolerance).first;

        qreal leadingEdgePerc = _profile->pointAtPercent(t_profileLE).x() / profileLength;
        qreal trailingEdgePerc = _profile->pointAtPercent(t_profileTE).x() / profileLength;

        qreal xLE = outlineLeadingEdge.x();
        qreal xTE = outlineTrailingEdge.x();
        leadingEdgePnts.append(new QPointF(xLE +(leadingEdgePerc * (xTE - xLE)), outlineLeadingEdge.y()));
        trailingEdgePnts.append(new QPointF(xLE +(trailingEdgePerc * (xTE - xLE)), outlineTrailingEdge.y()));

        // set points to 0 when closer than 1 manhattan pixel
        if ((*(leadingEdgePnts.last()) - *(trailingEdgePnts.last())).manhattanLength() < 1)
        {
            delete leadingEdgePnts[i];
            delete trailingEdgePnts[i];
            leadingEdgePnts.last() = 0;
            trailingEdgePnts.last() = 0;
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
//                createSplinePath(leadingEdgePnts, trailingEdgePnts, firstIndex, i, overhauser);
            }
        }
    }

    for (int i = 0; i<pointCount; i++)
    {
        if (leadingEdgePnts[i] != 0)
        {
            delete leadingEdgePnts[i];
            delete trailingEdgePnts[i];
        }
    }
}

ContourCalculator::~ContourCalculator()
{
}

void ContourCalculator::sampleThickess(qreal sectionHeightArray[], qreal thicknessArray[])
{
    qreal increment = qreal(1) / _sectionCount;
    qreal t = 0;
    qreal height = _thickness->pointAtPercent(1).x();
    qreal maxThickness = qMin(_thickness->minY(), _thickness->pointAtPercent(0).y());
    for (int i=0; i<_sectionCount; i++)
    {
        QPointF p = _thickness->pointAtPercent(t);

        sectionHeightArray[i] = p.x() / height;
        thicknessArray[i] = p.y() / maxThickness;
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

    QVarLengthArray<qreal, INITCNT> t_data(pointCount);
    for (int i = 0; i < t_data.count(); i++)
        t_data[i] = i;

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
        x = hrlib::spline_b_val(pointCount, t_data.data(), points_x.data(), t_val);
        y = hrlib::spline_b_val(pointCount, t_data.data(), points_y.data(), t_val);
        _result->moveTo(x, y);
        for (int i = 1; i <= _resolution; i++)
        {
            t_val += t_valStep;
            x = hrlib::spline_b_val(pointCount, t_data.data(), points_x.data(), t_val);
            y = hrlib::spline_b_val(pointCount, t_data.data(), points_y.data(), t_val);
            _result->lineTo(x, y);
        }
        break;

    case overhauser:
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
