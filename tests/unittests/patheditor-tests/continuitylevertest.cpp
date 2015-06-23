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

#include "continuitylevertest.hpp"
#include "submodules/qtestrunner/qtestrunner.hpp"

#include "continuitylever.hpp"
#include "curvepoint.hpp"
#include "controlpoint.h"

using namespace std;
using namespace patheditor;

void ContinuityLeverTest::testLeveragingFollowingPoints()
{
    shared_ptr<CurvePoint> leverPnt(new CurvePoint(0,0));
    shared_ptr<ControlPoint> cPnt1(new ControlPoint(-1,0));
    shared_ptr<ControlPoint> cPnt2(new ControlPoint(1,0));

    shared_ptr<CurvePoint> fPnt3(new CurvePoint(-2, 0));

    leverPnt->addFollowingPoint(cPnt1);
    leverPnt->addFollowingPoint(cPnt2);
    leverPnt->addFollowingPoint(fPnt3);

    shared_ptr<ContinuityLever> lever(new ContinuityLever(leverPnt.get()));
    cPnt1->setRestrictor(lever);
    cPnt2->setRestrictor(lever);
    // don't set restrictor on fPnt3 as it should follow changes to cPnt1 and cPnt2
    // by being a follwing point of leverPnt


    QCOMPARE(cPnt2->x(), 1.0);
    QCOMPARE(cPnt2->y(), 0.0);
    QCOMPARE(fPnt3->x(), -2.0);
    QCOMPARE(fPnt3->y(), 0.0);

    // rotate 45 degrees
    cPnt1->setRestrictedPos(-M_SQRT1_2,-M_SQRT1_2);
    QCOMPARE(cPnt2->x(), M_SQRT1_2);
    QCOMPARE(cPnt2->y(), M_SQRT1_2);
    QCOMPARE(fPnt3->x(), (-2*M_SQRT1_2));
    QCOMPARE(fPnt3->y(), (-2*M_SQRT1_2));

    // rotate to 90 degrees
    cPnt1->setRestrictedPos(0,-1);
    QCOMPARE(cPnt2->x(), 0.0);
    QCOMPARE(cPnt2->y(), 1.0);
    QCOMPARE(fPnt3->x(), 0.0);
    QCOMPARE(fPnt3->y(), -2.0);

    // rotate back 45 degrees
    cPnt1->setRestrictedPos(-M_SQRT1_2,-M_SQRT1_2);
    QCOMPARE(cPnt2->x(), M_SQRT1_2);
    QCOMPARE(cPnt2->y(), M_SQRT1_2);
    QCOMPARE(fPnt3->x(), (-2*M_SQRT1_2));
    QCOMPARE(fPnt3->y(), (-2*M_SQRT1_2));
}

QTR_ADD_TEST(ContinuityLeverTest)
