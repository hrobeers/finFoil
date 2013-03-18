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

#ifndef QUADRANTRESTRICTOR_H
#define QUADRANTRESTRICTOR_H

#include "patheditorfwd/patheditorfwd.h"

#include <QPointF>
#include "restrictor.h"
#include "../hrlib/exceptions.h"

namespace patheditor
{
    struct Quadrants
    {
        enum e
        {
            None = 0,
            I = 1,
            II = 2,
            III = 4,
            IV = 8
        };
    };

    class QuadrantRestrictor : public Restrictor
    {
    public:
        explicit QuadrantRestrictor(int quadrants);
        explicit QuadrantRestrictor(QPointF &origin, int quadrants);

        virtual void restrictCoordinate(qreal *x, qreal *y);

        virtual ~QuadrantRestrictor() {}

    private:
        QPointF _origin;
        int _quadrants;
    };
}

#endif // QUADRANTRESTRICTOR_H
