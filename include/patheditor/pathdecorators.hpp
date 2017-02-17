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

#ifndef PATHDECORATORS_HPP
#define PATHDECORATORS_HPP

#include "hrlib/fwd/qtfwd.h"

#include "hrlib/patterns/decorator.hpp"
#include "patheditor/ipath.hpp"

namespace patheditor
{
    class PathScaleDecorator : public hrlib::patterns::Decorator<IPath>
    {
    private:
        qreal _sx, _sy;

    public:
        explicit PathScaleDecorator(IPath *path, qreal sx, qreal sy) :
            Decorator(path), _sx(sx), _sy(sy) {}
        explicit PathScaleDecorator(std::shared_ptr<IPath> path, qreal sx, qreal sy) :
            Decorator(std::move(path)), _sx(sx), _sy(sy) {}
        explicit PathScaleDecorator(std::unique_ptr<IPath> path, qreal sx, qreal sy) :
            Decorator(std::move(path)), _sx(sx), _sy(sy) {}

        virtual QPointF pointAtPercent(qreal t) const override;

        virtual qreal minX(qreal *t_top = 0) const override;
        virtual qreal maxX(qreal *t_top = 0) const override;
        virtual qreal minY(qreal *t_top = 0) const override;
        virtual qreal maxY(qreal *t_top = 0) const override;

        virtual ~PathScaleDecorator() {}
    };
}

#endif // PATHDECORATORS_HPP
