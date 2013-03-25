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
    QQueue<QPointF> leadingEdgeQueue;
    QStack<QPointF> trailingEdgeStack;

    for (int i=0; i<_sectionCount; i++)
    {
        yOutline.setOffset(sectionHeightArray[i] * y_top);
        qreal t_outlineLeadingEdge = hrlib::Brent::zero(0, t_top, _tTol, yOutline);
        qreal t_outlineTrailingEdge = hrlib::Brent::zero(t_top, 1, _tTol, yOutline);
        QPointF outlineLeadingEdge = _outline->pointAtPercent(t_outlineLeadingEdge);
        QPointF outlineTrailingEdge = _outline->pointAtPercent(t_outlineTrailingEdge);

        qreal offsetPercent = _percContourHeight / thicknessArray[i];
        if (offsetPercent > 1) break;
        qreal profileOffset = offsetPercent * y_profileTop;
        yProfile.setOffset(profileOffset);
        qreal t_profileLE = hrlib::Brent::zero(0, t_profileTop, _tTol, yProfile);
        qreal t_profileTE = hrlib::Brent::zero(t_profileTop, 1, _tTol, yProfile);
        qreal leadingEdgePerc = _profile->pointAtPercent(t_profileLE).x() / profileLength;
        qreal trailingEdgePerc = _profile->pointAtPercent(t_profileTE).x() / profileLength;

        qreal xLE = outlineLeadingEdge.x();
        qreal xTE = outlineTrailingEdge.x();
        QPointF leadingEdge(xLE +(leadingEdgePerc * (xTE - xLE)), outlineLeadingEdge.y());
        QPointF trailingEdge(xLE +(trailingEdgePerc * (xTE - xLE)), outlineTrailingEdge.y());

        leadingEdgeQueue.enqueue(leadingEdge);
        trailingEdgeStack.push(trailingEdge);
    }

    _result->moveTo(leadingEdgeQueue.dequeue());
    while (!leadingEdgeQueue.empty()) _result->lineTo(leadingEdgeQueue.dequeue());
    while (!trailingEdgeStack.empty()) _result->lineTo(trailingEdgeStack.pop());
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
