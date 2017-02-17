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

#include "patheditor/continuitylever.hpp"

#include <cmath>
#include "patheditor/pathpoint.hpp"
#include "patheditor/linerestrictor.hpp"

using namespace std;
using namespace patheditor;

namespace {
    qreal length(const QPointF &pnt)
    {
        return sqrt(pow(pnt.x(),2) + pow(pnt.y(),2));
    }
}

void ContinuityLever::restrictCoordinate(qreal *x, qreal *y)
{
    QPointF leverPoint2(*x, *y);

    foreach(std::weak_ptr<PathPoint> followingPoint, _leverPoint->followingPoints())
    {
        // Rotate angle around _leverPoint

        shared_ptr<PathPoint> fPnt = followingPoint.lock();

        QPointF closestPnt = LineRestrictor::closestPoint(*fPnt, *_leverPoint, leverPoint2);

        // rescale
        QPointF originalDiff = *fPnt - *_leverPoint;
        QPointF newDiff = closestPnt - *_leverPoint;
        qreal factor = length(originalDiff) / length(newDiff);
        QPointF scaled = newDiff * factor;
        QPointF newPos = scaled + *_leverPoint;

        fPnt->setPos(newPos.x(), newPos.y());
    }
}
