/****************************************************************************
  
 Copyright (c) 2013, Hans Robeers
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

#include "contourcalculator.h"

#include <QVarLengthArray>
#include <QPainterPath>
#include <qmath.h>
#include "pathfunctors.h"
#include "hrlib/math/spline.hpp"

#define INITCNT 256

using namespace foillogic;
using namespace patheditor;

ContourCalculator::ContourCalculator(qreal percContourHeight, Foil *foil, QPainterPath *result, bool fast)
{
    _symmetric = foil->symmetry();
    _outline = foil->outline().data();
    _thickness = foil->thickness().data();


    _percContourHeight = percContourHeight;
    _result = result;


    // select the profile
    if (_percContourHeight < 0)
        _profile = foil->botProfile().data();
    else
        _profile = foil->topProfile().data();

    if (fast)
    {
        _sectionCount = INITCNT / 2;
        _resolution = 500;
        _tTol = 0.0001;
        _fTol = 0.001;
    }
    else
    {
        _sectionCount = INITCNT * 2;
        _resolution = 2000;
        _tTol = 0.00001;
        _fTol = 0.0001;
    }
}

void ContourCalculator::run()
{
    // discretise and normalise the thickness profile in different cross sections
    QVarLengthArray<qreal, INITCNT> sectionHeightArray(_sectionCount);
    QVarLengthArray<qreal, INITCNT> thicknessArray(_sectionCount);
    sampleThickess(sectionHeightArray.data(), thicknessArray.data());

    // create the pathfunctors
    f_yValueAtPercentPath yOutline(_outline);
    f_yValueAtPercentPath yTopProfile(_profile);

    // find the top of the outline
    qreal t_top = 0.5; // start value
    qreal y_top = _outline->minY(&t_top, _tTol);

    // find dimensions of the profile
    qreal t_profileTop = 0.3; // start value
    // TODO maybe take total thickness as a reference for profileOffset (see usages of y_profileTop)
    qreal y_profileTop = (_percContourHeight > 0) ? _profile->minY(&t_profileTop, _tTol) : _profile->maxY(&t_profileTop, _tTol);
    qreal profileLength = _profile->pointAtPercent(1).x();

    //
    // calculate the contour points
    //
    QVarLengthArray<QPointF*, INITCNT> leadingEdgePnts(_sectionCount);
    QVarLengthArray<QPointF*, INITCNT> trailingEdgePnts(_sectionCount);

    for (int i=0; i<_sectionCount; i++)
    {
        qreal thicknessOffsetPercent = _percContourHeight / thicknessArray[i];
        if (qAbs(thicknessOffsetPercent) > 1)
        {
            leadingEdgePnts[i] = 0;
            trailingEdgePnts[i] = 0;
            continue;
        }

        yOutline.setOffset(sectionHeightArray[i] * y_top);
        qreal t_outlineLeadingEdge = hrlib::Brent::zero(0, t_top, _tTol, yOutline);
        qreal t_outlineTrailingEdge = hrlib::Brent::zero(t_top, 1, _tTol, yOutline);
        QPointF outlineLeadingEdge = _outline->pointAtPercent(t_outlineLeadingEdge);
        QPointF outlineTrailingEdge = _outline->pointAtPercent(t_outlineTrailingEdge);

        qreal profileOffset = thicknessOffsetPercent * y_profileTop;
        yTopProfile.setOffset(profileOffset);
        qreal t_profileLE = hrlib::Brent::zero(0, t_profileTop, _tTol, yTopProfile);
        qreal t_profileTE = hrlib::Brent::zero(t_profileTop, 1, _tTol, yTopProfile);
        qreal leadingEdgePerc = _profile->pointAtPercent(t_profileLE).x() / profileLength;
        qreal trailingEdgePerc = _profile->pointAtPercent(t_profileTE).x() / profileLength;

        qreal xLE = outlineLeadingEdge.x();
        qreal xTE = outlineTrailingEdge.x();
        leadingEdgePnts[i] = new QPointF(xLE +(leadingEdgePerc * (xTE - xLE)), outlineLeadingEdge.y());
        trailingEdgePnts[i] = new QPointF(xLE +(trailingEdgePerc * (xTE - xLE)), outlineTrailingEdge.y());
    }

    int firstIndex = 0;
    for (int i=0; i<_sectionCount; i++)
    {
        if (leadingEdgePnts[i] != 0)
        {
            if (i == 0 || leadingEdgePnts[i-1] == 0) // first or nullptr
                firstIndex = i;

            if (i == _sectionCount-1 || leadingEdgePnts[i+1] == 0)
            {
//                createLinePath(leadingEdgePnts, trailingEdgePnts, firstIndex, i);
                createSplinePath(leadingEdgePnts.data(), trailingEdgePnts.data(), firstIndex, i, bSpline);
//                createSplinePath(leadingEdgePnts, trailingEdgePnts, firstIndex, i, overhauser);
            }
        }
    }

    for (int i = 0; i < _sectionCount; i++)
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
    qreal baseThickness = 1;
    qreal height = 1;
    for (int i=0; i<_sectionCount; i++)
    {

        if (i==0)
        {
            baseThickness = _thickness->pointAtPercent(0).y();
            height = _thickness->pointAtPercent(1).x();

            sectionHeightArray[0] = 0;
            thicknessArray[0] = 1;
        }
        else
        {
            t += increment;
            QPointF p = _thickness->pointAtPercent(t);

            sectionHeightArray[i] = p.x() / height;
            thicknessArray[i] = p.y() / baseThickness;
        }
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
