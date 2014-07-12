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
    enum Dimension { X, Y };
    enum Multiplier { Min = 1, Max = -1 };

    // Declaration of the generic path evaluation implementation type
    template <int Dimension, int Multiplier>
    class f_ValueAtPercentPathImpl;

    // Specific implementation for the X dimension
    template <int Multiplier>
    class f_ValueAtPercentPathImpl<X, Multiplier>
    {
    private:
        Path const *_path;
        qreal _offset;

    public:
        explicit f_ValueAtPercentPathImpl(Path const *path, qreal offset = 0) :
            _path(path), _offset(offset) {}

        qreal operator ()(qreal t){
            return (_path->pointAtPercent(t).x() - _offset) * Multiplier;
        }

        void setOffset(qreal offset){
            _offset = offset;
        }
    };

    // Specific implementation for the Y dimension
    template <int Multiplier>
    class f_ValueAtPercentPathImpl<Y, Multiplier>
    {
    private:
        Path const *_path;
        qreal _offset;

    public:
        explicit f_ValueAtPercentPathImpl(Path const *path, qreal offset = 0) :
            _path(path), _offset(offset) {}

        qreal operator ()(qreal t){
            return (_path->pointAtPercent(t).y() - _offset) * Multiplier;
        }

        void setOffset(qreal offset){
            _offset = offset;
        }
    };

    // Path evaluation function with default Multiplier
    template <int Dimension, int Multiplier = Min>
    class f_ValueAtPercentPath : public f_ValueAtPercentPathImpl<Dimension, Multiplier>
    {
    public:
        explicit f_ValueAtPercentPath(Path const *path, qreal offset = 0) :
            f_ValueAtPercentPathImpl<Dimension, Multiplier>(path, offset) {}
    };

    // Functor for the root solving stop condition
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
