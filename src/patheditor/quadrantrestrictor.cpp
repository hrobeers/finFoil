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

#include "quadrantrestrictor.h"

#define notIn(x) !(_quadrants & x)

using namespace patheditor;

QuadrantRestrictor::QuadrantRestrictor(int quadrants)
{
    _quadrants = quadrants;
}

QuadrantRestrictor::QuadrantRestrictor(QPointF &origin, int quadrants)
{
    _origin = origin;
    _quadrants = quadrants;
}

void QuadrantRestrictor::restrictCoordinate(qreal *x, qreal *y)
{
    switch (_quadrants)
    {

    //
    // Single Quadrants
    //

    case (Quadrants::I):
        if (*y > _origin.ry())
            *y = _origin.ry();
        if (*x < _origin.rx())
            *x = _origin.rx();
        break;

    case (Quadrants::II):
        if (*y > _origin.ry())
            *y = _origin.ry();
        if (*x > _origin.rx())
            *x = _origin.rx();
        break;

    case (Quadrants::III):
        if (*y < _origin.ry())
            *y = _origin.ry();
        if (*x > _origin.rx())
            *x = _origin.rx();
        break;

    case (Quadrants::IV):
        if (*y < _origin.ry())
            *y = _origin.ry();
        if (*x < _origin.rx())
            *x = _origin.rx();
        break;

    //
    // Half planes
    //

    case (Quadrants::I | Quadrants::II):
        if (*y > _origin.ry())
            *y = _origin.ry();
        break;

    case (Quadrants::III | Quadrants::IV):
        if (*y < _origin.ry())
            *y = _origin.ry();
        break;

    case (Quadrants::I | Quadrants::IV):
        if (*x < _origin.rx())
            *x = _origin.rx();
        break;

    case (Quadrants::II | Quadrants::III):
        if (*x > _origin.rx())
            *x = _origin.rx();
        break;

    //
    // default: All other combinations by moving out of quadrant
    //

    default:
        // TODO throw unsupported exception
        break;
    }
}
