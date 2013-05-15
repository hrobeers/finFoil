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

#ifndef PATHFUNCTORS_H
#define PATHFUNCTORS_H

#include "hrlibfwd/qtfwd.h"

#include "hrlib/math/brent.hpp"
#include "editablepath.h"

namespace patheditor
{
    class f_yValueAtPercentPPath : public hrlib::func_offset_base
    {
    private:
        QPainterPath *_path;
        qreal _offset;

    public:
        explicit f_yValueAtPercentPPath(QPainterPath *path, qreal offset = 0){
            _path = path;
            _offset = offset;
        }

        virtual qreal operator ()(qreal t){
            return _path->pointAtPercent(t).y() - _offset;
        }

        virtual void setOffset(qreal offset){
            _offset = offset;
        }
    };

    class f_yValueAtPercentEPath : public hrlib::func_offset_base
    {
    private:
        EditablePath *_path;
        qreal _offset;

    public:
        explicit f_yValueAtPercentEPath(EditablePath *path, qreal offset = 0){
            _path = path;
            _offset = offset;
        }

        virtual qreal operator ()(qreal t){
            return _path->pointAtPercent(t).y() - _offset;
        }

        virtual void setOffset(qreal offset){
            _offset = offset;
        }
    };
}

#endif // PATHFUNCTORS_H
