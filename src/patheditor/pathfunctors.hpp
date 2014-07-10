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

#ifndef PATHFUNCTORS_HPP
#define PATHFUNCTORS_HPP

#include "hrlibfwd/qtfwd.h"

#include "path.h"

namespace patheditor
{
    class f_xValueAtPercentPath
    {
    private:
        Path const *_path;
        qreal _multiplier;
        qreal _offset;

    public:
        explicit f_xValueAtPercentPath(Path const *path, qreal multiplier = 1, qreal offset = 0) :
            _path(path), _multiplier(multiplier), _offset(offset) {}

        qreal operator ()(qreal t){
            return (_path->pointAtPercent(t).x() - _offset) * _multiplier;
        }

        void setMultiplier(qreal multiplier){
            _multiplier = multiplier;
        }

        void setOffset(qreal offset){
            _offset = offset;
        }
    };

    class f_yValueAtPercentPath
    {
    private:
        Path const *_path;
        qreal _multiplier;
        qreal _offset;

    public:
        explicit f_yValueAtPercentPath(Path const *path, qreal multiplier = 1, qreal offset = 0) :
            _path(path), _multiplier(multiplier), _offset(offset) {}

        qreal operator ()(qreal t){
            return (_path->pointAtPercent(t).y() - _offset) * _multiplier;
        }

        void setMultiplier(qreal multiplier){
            _multiplier = multiplier;
        }

        void setOffset(qreal offset){
            _offset = offset;
        }
    };

    template <class T>
    class f_diffTol
    {
    private:
        T _tolerance;

    public:
        explicit f_diffTol(T tolerance) : _tolerance(tolerance) {}

        bool operator ()(const T& min, const T& max){
            return (max - min) < _tolerance;
        }
    };
}

#endif // PATHFUNCTORS_HPP
