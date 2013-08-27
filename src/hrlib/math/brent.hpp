/****************************************************************************

 This code is distributed under the GNU LGPL license.
 
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

class func_mult_offset_base : public func_base{
public:
  virtual qreal operator() (qreal) = 0;
  virtual void setMultiplier(qreal multiplier) = 0;
  virtual void setOffset(qreal offset) = 0;
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
