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

#ifndef BRENT_H
#define BRENT_H

#include <vector>
#include <QtGlobal>
namespace hrlib {

class func_base{
public:
  virtual qreal operator() (qreal) = 0;
};

class monicPoly : public func_base {
public:
  std::vector<qreal> coeff;
  virtual qreal operator() (qreal x);
// constructors:
  monicPoly(const size_t degree)
   : coeff(degree) {}
  monicPoly(const std::vector<qreal>& v)
   : coeff(v) {}
  monicPoly(const qreal* c, size_t degree)
   : coeff(std::vector<qreal>(c, c+degree)) {}
};

class Poly : public func_base {
public:
  std::vector<qreal> coeff;    // a vector of size nterms i.e. 1+degree
  virtual qreal operator() (qreal x);
// constructors:
  Poly(const size_t degree)
   : coeff(1+degree) {}
  Poly(const std::vector<qreal>& v)
   : coeff(v) {}
  Poly(const qreal* c, size_t degree)
   : coeff(std::vector<qreal>(c, 1+c+degree)) {}
};

class Brent {
public:
static qreal glomin ( qreal a, qreal b, qreal c, qreal m, qreal e, qreal t,
  func_base& f, qreal &x );
static qreal local_min ( qreal a, qreal b, qreal t, func_base& f,
  qreal &x );
static qreal local_min_rc ( qreal &a, qreal &b, int &status, qreal value );
static qreal r8_abs ( qreal x );
static qreal r8_epsilon ( );
static qreal r8_max ( qreal x, qreal y );
static qreal r8_sign ( qreal x );
static void timestamp ( );
static qreal zero ( qreal a, qreal b, qreal t, func_base& f );
static void zero_rc ( qreal a, qreal b, qreal t, qreal &arg, int &status,
  qreal value );

// === simple wrapper functions
// === for convenience and/or compatibility
static qreal glomin ( qreal a, qreal b, qreal c, qreal m, qreal e, qreal t,
  qreal f ( qreal x ), qreal &x );
static qreal local_min ( qreal a, qreal b, qreal t, qreal f ( qreal x ),
  qreal &x );
static qreal zero ( qreal a, qreal b, qreal t, qreal f ( qreal x ) );
};

}
#endif // BRENT_H
