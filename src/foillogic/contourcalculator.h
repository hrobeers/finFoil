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

#ifndef CONTOURCALCULATOR_H
#define CONTOURCALCULATOR_H

#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"

#include <QRunnable>

namespace foillogic
{
    class ContourCalculator : public QRunnable
    {
    public:
        explicit ContourCalculator(qreal percContourHeight, patheditor::Path* outline, patheditor::Path* profile,
                                   patheditor::Path* thickness, QPainterPath* result, bool fast = false);

        virtual void run();

        virtual ~ContourCalculator();

    private:
        enum SplineFunction { bSpline, overhauser };

        qreal _percContourHeight;
        patheditor::Path* _outline;
        patheditor::Path* _profile;
        patheditor::Path* _thickness;
        QPainterPath *_result;

        int _sectionCount;
        int _resolution;
        qreal _tTol;
        qreal _fTol;

        void sampleThickess(qreal sectionHeightArray[], qreal thicknessArray[]);
        void createLinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex);
        void createSplinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex, SplineFunction splineFunction);
    };
}

#endif // CONTOURCALCULATOR_H
