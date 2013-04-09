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

#include <QPainterPath>
#include <QQueue>
#include <QStack>
#include <qmath.h>
#include "pathfunctors.h"

using namespace fineditors;

ContourCalculator::ContourCalculator(qreal percContourHeight, QPainterPath *outline, QPainterPath *profile,
                                     QPainterPath *thickness, QPainterPath *result)
{
    _percContourHeight = percContourHeight;
    _outline = outline;
    _profile = profile;
    _thickness = thickness;
    _result = result;

    _sectionCount = 20;
    _tTol = 0.0001;
    _fTol = 0.01;
}

void ContourCalculator::run()
{
    // discretise and normalise the thickness profile in different cross sections
    qreal sectionHeightArray[_sectionCount];
    qreal thicknessArray[_sectionCount];
    sampleThickess(sectionHeightArray, thicknessArray);

    // find the top of the outline
    f_yValueAtPercent yOutline(_outline);
    qreal t_top;
//    qreal y_top = hrlib::Brent::local_min(0, 1, 0.01, yOutline, t_top);
    qreal y_top = hrlib::Brent::glomin(0, 1, 0.5, 10, _fTol, _tTol, yOutline, t_top);

    // find dimensions of the profile
    f_yValueAtPercent yProfile(_profile);
    qreal t_profileTop;
    qreal y_profileTop = hrlib::Brent::glomin(0, 1, 0.35, 3, _fTol, _tTol, yProfile, t_profileTop);
    qreal profileLength = _profile->pointAtPercent(1).x();

    //
    // calculate the contour points
    //
    QPointF* leadingEdgePnts[_sectionCount];
    QPointF* trailingEdgePnts[_sectionCount];

    for (int i=0; i<_sectionCount; i++)
    {
        qreal thicknessOffsetPercent = _percContourHeight / thicknessArray[i];
        if (thicknessOffsetPercent > 1)
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
        yProfile.setOffset(profileOffset);
        qreal t_profileLE = hrlib::Brent::zero(0, t_profileTop, _tTol, yProfile);
        qreal t_profileTE = hrlib::Brent::zero(t_profileTop, 1, _tTol, yProfile);
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
            if (i == 0 || leadingEdgePnts[i-1] == 0)
                firstIndex = i;

            if (i == _sectionCount-1 || leadingEdgePnts[i+1] == 0)
                createCubicPath(leadingEdgePnts, trailingEdgePnts, firstIndex, i);
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

            sectionHeightArray[i] = p.rx() / height;
            thicknessArray[i] = p.ry() / baseThickness;
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

void ContourCalculator::createCubicPath(QPointF *leadingEdgePnts[], QPointF *trailingEdgePnts[], int firstIndex, int lastIndex)
{
    int edgeCount = lastIndex - firstIndex + 1;
    int totalCount = 2 * edgeCount;
    int extCount = totalCount + 2;
    int cPntCount = totalCount * 2 - 2;

    QPointF* curve[extCount];
    for (int i = 0; i < edgeCount; i++)
    {
        curve[i + 1] = leadingEdgePnts[firstIndex + i];
        curve[edgeCount + i + 1] = trailingEdgePnts[lastIndex - i];
    }
    curve[0] = curve[1];
    curve[extCount - 1] = curve[extCount - 2];

    QPointF c[extCount - 1];
    qreal len[extCount - 1];

    for (int i = 0; i < extCount - 1; i++)
    {
        c[i] = (*curve[i] + *curve[i+1]) / 2;
        QPointF diff = *curve[i+1] - *curve[i];
        len[i] = qSqrt(qPow(diff.x(),2) + qPow(diff.y(),2));
    }

    QPointF cPnt[cPntCount];
    for (int i = 0; i < cPntCount; i++)
    {
        // TODO move k & m out. Calculated twice.
        if (i % 2 == 0)
        {
            int index = i / 2;
            qreal k = len[index] / (len[index] + len[index + 1]);
            QPointF m = c[index] + (c[index + 1] - c[index]) * k;
            cPnt[i] = (c[index + 1] - m) * 0.4 + *curve[index + 1];
        }
        else
        {
            int index = (i + 1) / 2;
            qreal k = len[index] / (len[index] + len[index + 1]);
            QPointF m = c[index] + (c[index + 1] - c[index]) * k;
            cPnt[i] = (c[index] - m) * 0.4 + *curve[index + 1];
        }
    }

    _result->moveTo(*curve[1]);
    for (int i = 0; i < totalCount - 1; i++)
    {
        int cIndex = i + i;
        _result->cubicTo(cPnt[cIndex], cPnt[cIndex + 1], *curve[i + 2]);
    }
}
