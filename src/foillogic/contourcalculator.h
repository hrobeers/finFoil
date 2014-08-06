/****************************************************************************

 Copyright (c) 2013, Hans Robeers
 All rights reserved.

 This code is distributed under the GNU LGPL version 2.1.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

****************************************************************************/

#ifndef CONTOURCALCULATOR_H
#define CONTOURCALCULATOR_H

#include "foillogicfwd/foillogicfwd.h"
#include "hrlibfwd/qtfwd.h"
#include "patheditorfwd/patheditorfwd.h"

#include <QRunnable>

namespace foillogic
{
    class ContourCalculator : public QRunnable
    {
    public:
        explicit ContourCalculator(qreal percContourHeight, Foil* foil, QPainterPath* result,
                                   Side::e side = Side::Top, bool fast = false);

        virtual void run();

        virtual ~ContourCalculator() {}

    private:
        enum SplineFunction { bSpline, overhauser };

        Side::e _side;
        qreal _percContourHeight;
        patheditor::Path* _profile;
        patheditor::Path* _outline;
        patheditor::Path* _thickness;
        QPainterPath *_result;

        int _sectionCount;
        int _resolution;
        qreal _tTol;

        void sampleThickess(qreal sectionHeightArray[], qreal thicknessArray[]);
        void createLinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex);
        void createSplinePath(QPointF* leadingEdgePnts[], QPointF* trailingEdgePnts[], int firstIndex, int lastIndex, SplineFunction splineFunction);
    };
}

#endif // CONTOURCALCULATOR_H
