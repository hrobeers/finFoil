/****************************************************************************

 Copyright (c) 2014, Hans Robeers
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

#include "patheditor/pathdecorators.hpp"

#include <QPointF>

using namespace patheditor;

// TODO header only

QPointF PathScaleDecorator::pointAtPercent(qreal t) const
{
    auto pnt = _target->pointAtPercent(t);
    pnt.rx() *= _sx;
    pnt.ry() *= _sy;
    return pnt;
}

qreal PathScaleDecorator::minX(qreal *t_top) const
{
    return (_sx>0 ? _target->minX(t_top) : _target->maxX(t_top) ) * _sx;
}

qreal PathScaleDecorator::maxX(qreal *t_top) const
{
    return (_sx>0 ? _target->maxX(t_top) : _target->minX(t_top)) * _sx;
}

qreal PathScaleDecorator::minY(qreal *t_top) const
{
    return (_sy>0 ? _target->minY(t_top) : _target->maxY(t_top)) * _sy;
}

qreal PathScaleDecorator::maxY(qreal *t_top) const
{
    return (_sy>0 ? _target->maxY(t_top) : _target->minY(t_top)) * _sy;
}

std::vector<std::vector<QPointF>> PathScaleDecorator::bezierItems() const
{
  std::vector<std::vector<QPointF>> retVal;
  for (auto pi : _target->bezierItems()) {
    std::vector<QPointF> i;
    for (auto p : pi)
      i.push_back({p.x()*_sx, p.y()*_sy});
    retVal.push_back(i);
  }
  return retVal;
}
