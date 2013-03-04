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
namespace hrlib {

class func_base{
public:
  virtual double operator() (double) = 0;
};

class monicPoly : public func_base {
public:
  std::vector<double> coeff;
  virtual double operator() (double x);
// constructors:
  monicPoly(const size_t degree)
   : coeff(degree) {}
  monicPoly(const std::vector<double>& v)
   : coeff(v) {}
  monicPoly(const double* c, size_t degree)
   : coeff(std::vector<double>(c, c+degree)) {}
};

class Poly : public func_base {
public:
  std::vector<double> coeff;    // a vector of size nterms i.e. 1+degree
  virtual double operator() (double x);
// constructors:
  Poly(const size_t degree)
   : coeff(1+degree) {}
  Poly(const std::vector<double>& v)
   : coeff(v) {}
  Poly(const double* c, size_t degree)
   : coeff(std::vector<double>(c, 1+c+degree)) {}
};

class Brent {
public:
static double glomin ( double a, double b, double c, double m, double e, double t,
  func_base& f, double &x );
static double local_min ( double a, double b, double t, func_base& f,
  double &x );
static double local_min_rc ( double &a, double &b, int &status, double value );
static double r8_abs ( double x );
static double r8_epsilon ( );
static double r8_max ( double x, double y );
static double r8_sign ( double x );
static void timestamp ( );
static double zero ( double a, double b, double t, func_base& f );
static void zero_rc ( double a, double b, double t, double &arg, int &status,
  double value );

// === simple wrapper functions
// === for convenience and/or compatibility
static double glomin ( double a, double b, double c, double m, double e, double t,
  double f ( double x ), double &x );
static double local_min ( double a, double b, double t, double f ( double x ),
  double &x );
static double zero ( double a, double b, double t, double f ( double x ) );
};

}
#endif // BRENT_H
