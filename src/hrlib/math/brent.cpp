/****************************************************************************

 This code is distributed under the GNU LGPL license.

****************************************************************************/

# include <iostream>
# include <cmath>
# include <qmath.h>
# include <numeric>

using namespace std;

# include "brent.hpp"

namespace hrlib
{

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

//****************************************************************************80

const qreal eps = std::numeric_limits<qreal>::epsilon();
const qreal sqrt_eps = qSqrt(std::numeric_limits<qreal>::epsilon());

//****************************************************************************80
//
//  Purpose:
//
//    Machine accuracy constants.
//
//  Discussion:
//
//    Replace the original r8_epsilon() function as it is to expensive to
//    calculate at runtime.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    19 November 2014
//
//  Author:
//
//    Hans Robeers
//

//****************************************************************************80

qreal Brent::glomin ( qreal a, qreal b, qreal c, qreal m, qreal e, qreal t,
  func_base& f, qreal &x )

//****************************************************************************80
//
//  Purpose:
//
//    GLOMIN seeks a global minimum of a function F(X) in an interval [A,B].
//
//  Discussion:
//
//    This function assumes that F(X) is twice continuously differentiable
//    over [A,B] and that F''(X) <= M for all X in [A,B].
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 July 2011
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//  Parameters:
//
//    Input, qreal A, B, the endpoints of the interval.
//    It must be the case that A < B.
//
//    Input, qreal C, an initial guess for the global
//    minimizer.  If no good guess is known, C = A or B is acceptable.
//
//    Input, qreal M, the bound on the second derivative.
//
//    Input, qreal E, a positive tolerance, a bound for the
//    absolute error in the evaluation of F(X) for any X in [A,B].
//
//    Input, qreal T, a positive error tolerance.
//
//    Input, func_base& F, a user-supplied c++ functor whose
//    global minimum is being sought.  The input and output
//    of F() are of type qreal.
//
//    Output, qreal &X, the estimated value of the abscissa
//    for which F attains its global minimum value in [A,B].
//
//    Output, qreal GLOMIN, the value F(X).
//
{
  qreal a0;
  qreal a2;
  qreal a3;
  qreal d0;
  qreal d1;
  qreal d2;
  qreal h;
  int k;
  qreal m2;
  qreal p;
  qreal q;
  qreal qs;
  qreal r;
  qreal s;
  qreal sc;
  qreal y;
  qreal y0;
  qreal y1;
  qreal y2;
  qreal y3;
  qreal yb;
  qreal z0;
  qreal z1;
  qreal z2;

  a0 = b;
  x = a0;
  a2 = a;
  y0 = f ( b );
  yb = y0;
  y2 = f ( a );
  y = y2;

  if ( y0 < y )
  {
    y = y0;
  }
  else
  {
    x = a;
  }

  if ( m <= 0.0 || b <= a )
  {
    return y;
  }

  m2 = 0.5 * ( 1.0 + 16.0 * eps ) * m;

  if ( c <= a || b <= c )
  {
    sc = 0.5 * ( a + b );
  }
  else
  {
    sc = c;
  }

  y1 = f ( sc );
  k = 3;
  d0 = a2 - sc;
  h = 9.0 / 11.0;

  if ( y1 < y )
  {
    x = sc;
    y = y1;
  }
//
//  Loop.
//
  for ( ; ; )
  {
    d1 = a2 - a0;
    d2 = sc - a0;
    z2 = b - a2;
    z0 = y2 - y1;
    z1 = y2 - y0;
    r = d1 * d1 * z0 - d0 * d0 * z1;
    p = r;
    qs = 2.0 * ( d0 * z1 - d1 * z0 );
    q = qs;

    if ( k < 1000000 || y2 <= y )
    {
      for ( ; ; )
      {
        if ( q * ( r * ( yb - y2 ) + z2 * q * ( ( y2 - y ) + t ) ) <
          z2 * m2 * r * ( z2 * q - r ) )
        {
          a3 = a2 + r / q;
          y3 = f ( a3 );

          if ( y3 < y )
          {
            x = a3;
            y = y3;
          }
        }
        k = ( ( 1611 * k ) % 1048576 );
        q = 1.0;
        r = ( b - a ) * 0.00001 * ( qreal ) ( k );

        if ( z2 <= r )
        {
          break;
        }
      }
    }
    else
    {
      k = ( ( 1611 * k ) % 1048576 );
      q = 1.0;
      r = ( b - a ) * 0.00001 * ( qreal ) ( k );

      while ( r < z2 )
      {
        if ( q * ( r * ( yb - y2 ) + z2 * q * ( ( y2 - y ) + t ) ) <
          z2 * m2 * r * ( z2 * q - r ) )
        {
          a3 = a2 + r / q;
          y3 = f ( a3 );

          if ( y3 < y )
          {
            x = a3;
            y = y3;
          }
        }
        k = ( ( 1611 * k ) % 1048576 );
        q = 1.0;
        r = ( b - a ) * 0.00001 * ( qreal ) ( k );
      }
    }

    r = m2 * d0 * d1 * d2;
    s = qSqrt ( ( ( y2 - y ) + t ) / m2 );
    h = 0.5 * ( 1.0 + h );
    p = h * ( p + 2.0 * r * s );
    q = q + 0.5 * qs;
    r = - 0.5 * ( d0 + ( z0 + 2.01 * e ) / ( d0 * m2 ) );

    if ( r < s || d0 < 0.0 )
    {
      r = a2 + s;
    }
    else
    {
      r = a2 + r;
    }

    if ( 0.0 < p * q )
    {
      a3 = a2 + p / q;
    }
    else
    {
      a3 = r;
    }

    for ( ; ; )
    {
      a3 = std::max ( a3, r );

      if ( b <= a3 )
      {
        a3 = b;
        y3 = yb;
      }
      else
      {
        y3 = f ( a3 );
      }

      if ( y3 < y )
      {
        x = a3;
        y = y3;
      }

      d0 = a3 - a2;

      if ( a3 <= r )
      {
        break;
      }

      p = 2.0 * ( y2 - y3 ) / ( m * d0 );

      if ( ( 1.0 + 9.0 * eps ) * d0 <= std::abs ( p ) )
      {
        break;
      }

      if ( 0.5 * m2 * ( d0 * d0 + p * p ) <= ( y2 - y ) + ( y3 - y ) + 2.0 * t )
      {
        break;
      }
      a3 = 0.5 * ( a2 + a3 );
      h = 0.9 * h;
    }

    if ( b <= a3 )
    {
      break;
    }

    a0 = sc;
    sc = a2;
    a2 = a3;
    y0 = y1;
    y1 = y2;
    y2 = y3;
  }

  return y;
}
//****************************************************************************80

qreal Brent::local_min ( qreal a, qreal b, qreal t, func_base& f,
  qreal &x )

//****************************************************************************80
//
//  Purpose:
//
//    LOCAL_MIN seeks a local minimum of a function F(X) in an interval [A,B].
//
//  Discussion:
//
//    The method used is a combination of golden section search and
//    successive parabolic interpolation.  Convergence is never much slower
//    than that for a Fibonacci search.  If F has a continuous second
//    derivative which is positive at the minimum (which is not at A or
//    B), then convergence is superlinear, and usually of the order of
//    about 1.324....
//
//    The values EPS and T define a tolerance TOL = EPS * abs ( X ) + T.
//    F is never evaluated at two points closer than TOL.
//
//    If F is a unimodal function and the computed values of F are always
//    unimodal when separated by at least SQEPS * abs ( X ) + (T/3), then
//    LOCAL_MIN approximates the abscissa of the global minimum of F on the
//    interval [A,B] with an error less than 3*SQEPS*abs(LOCAL_MIN)+T.
//
//    If F is not unimodal, then LOCAL_MIN may approximate a local, but
//    perhaps non-global, minimum to the same accuracy.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 July 2011
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//  Parameters:
//
//    Input, qreal A, B, the endpoints of the interval.
//
//    Input, qreal T, a positive absolute error tolerance.
//
//    Input, func_base& F, a user-supplied c++ functor whose
//    local minimum is being sought.  The input and output
//    of F() are of type qreal.
//
//    Output, qreal &X, the estimated value of an abscissa
//    for which F attains a local minimum value in [A,B].
//
//    Output, qreal LOCAL_MIN, the value F(X).
//
{
  qreal c;
  qreal d = 0;
  qreal e;
  qreal fu;
  qreal fv;
  qreal fw;
  qreal fx;
  qreal m;
  qreal p;
  qreal q;
  qreal r;
  qreal sa;
  qreal sb;
  qreal t2;
  qreal tol;
  qreal u;
  qreal v;
  qreal w;
//
//  C is the square of the inverse of the golden ratio.
//
  c = 0.5 * ( 3.0 - qSqrt ( 5.0 ) );

  sa = a;
  sb = b;
  x = sa + c * ( b - a );
  w = x;
  v = w;
  e = 0.0;
  fx = f ( x );
  fw = fx;
  fv = fw;

  for ( ; ; )
  {
    m = 0.5 * ( sa + sb ) ;
    tol = sqrt_eps * std::abs ( x ) + t;
    t2 = 2.0 * tol;
//
//  Check the stopping criterion.
//
    if ( std::abs ( x - m ) <= t2 - 0.5 * ( sb - sa ) )
    {
      break;
    }
//
//  Fit a parabola.
//
    r = 0.0;
    q = r;
    p = q;

    if ( tol < std::abs ( e ) )
    {
      r = ( x - w ) * ( fx - fv );
      q = ( x - v ) * ( fx - fw );
      p = ( x - v ) * q - ( x - w ) * r;
      q = 2.0 * ( q - r );
      if ( 0.0 < q )
      {
        p = - p;
      }
      q = std::abs ( q );
      r = e;
      e = d;
    }

    if ( std::abs ( p ) < std::abs ( 0.5 * q * r ) &&
         q * ( sa - x ) < p &&
         p < q * ( sb - x ) )
    {
//
//  Take the parabolic interpolation step.
//
      d = p / q;
      u = x + d;
//
//  F must not be evaluated too close to A or B.
//
      if ( ( u - sa ) < t2 || ( sb - u ) < t2 )
      {
        if ( x < m )
        {
          d = tol;
        }
        else
        {
          d = - tol;
        }
      }
    }
//
//  A golden-section step.
//
    else
    {
      if ( x < m )
      {
        e = sb - x;
      }
      else
      {
        e = sa - x;
      }
      d = c * e;
    }
//
//  F must not be evaluated too close to X.
//
    if ( tol <= std::abs ( d ) )
    {
      u = x + d;
    }
    else if ( 0.0 < d )
    {
      u = x + tol;
    }
    else
    {
      u = x - tol;
    }

    fu = f ( u );
//
//  Update A, B, V, W, and X.
//
    if ( fu <= fx )
    {
      if ( u < x )
      {
        sb = x;
      }
      else
      {
        sa = x;
      }
      v = w;
      fv = fw;
      w = x;
      fw = fx;
      x = u;
      fx = fu;
    }
    else
    {
      if ( u < x )
      {
        sa = u;
      }
      else
      {
        sb = u;
      }

      if ( fu <= fw || w == x )
      {
        v = w;
        fv = fw;
        w = u;
        fw = fu;
      }
      else if ( fu <= fv || v == x || v== w )
      {
        v = u;
        fv = fu;
      }
    }
  }
  return fx;
}
//****************************************************************************80

qreal Brent::local_min_rc ( qreal &a, qreal &b, int &status, qreal value )

//****************************************************************************80
//
//  Purpose:
//
//    LOCAL_MIN_RC seeks a minimizer of a scalar function of a scalar variable.
//
//  Discussion:
//
//    This routine seeks an approximation to the point where a function
//    F attains a minimum on the interval (A,B).
//
//    The method used is a combination of golden section search and
//    successive parabolic interpolation.  Convergence is never much
//    slower than that for a Fibonacci search.  If F has a continuous
//    second derivative which is positive at the minimum (which is not
//    at A or B), then convergence is superlinear, and usually of the
//    order of about 1.324...
//
//    The routine is a revised version of the Brent local minimization
//    algorithm, using reverse communication.
//
//    It is worth stating explicitly that this routine will NOT be
//    able to detect a minimizer that occurs at either initial endpoint
//    A or B.  If this is a concern to the user, then the user must
//    either ensure that the initial interval is larger, or to check
//    the function value at the returned minimizer against the values
//    at either endpoint.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    17 July 2011
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//    David Kahaner, Cleve Moler, Steven Nash,
//    Numerical Methods and Software,
//    Prentice Hall, 1989,
//    ISBN: 0-13-627258-4,
//    LC: TA345.K34.
//
//  Parameters
//
//    Input/output, qreal &A, &B.  On input, the left and right
//    endpoints of the initial interval.  On output, the lower and upper
//    bounds for an interval containing the minimizer.  It is required
//    that A < B.
//
//    Input/output, int &STATUS, used to communicate between
//    the user and the routine.  The user only sets STATUS to zero on the first
//    call, to indicate that this is a startup call.  The routine returns STATUS
//    positive to request that the function be evaluated at ARG, or returns
//    STATUS as 0, to indicate that the iteration is complete and that
//    ARG is the estimated minimizer.
//
//    Input, qreal VALUE, the function value at ARG, as requested
//    by the routine on the previous call.
//
//    Output, qreal LOCAL_MIN_RC, the currently considered point.
//    On return with STATUS positive, the user is requested to evaluate the
//    function at this point, and return the value in VALUE.  On return with
//    STATUS zero, this is the routine's estimate for the function minimizer.
//
//  Local parameters:
//
//    C is the squared inverse of the golden ratio.
//
//    EPS is the square root of the relative machine precision.
//
{
  static qreal arg;
  static qreal c;
  static qreal d;
  static qreal e;
  static qreal fu;
  static qreal fv;
  static qreal fw;
  static qreal fx;
  static qreal midpoint;
  static qreal p;
  static qreal q;
  static qreal r;
  static qreal tol1;
  static qreal tol2;
  static qreal u;
  static qreal v;
  static qreal w;
  static qreal x;
//
//  STATUS (INPUT) = 0, startup.
//
  if ( status == 0 )
  {
    if ( b <= a )
    {
      cout << "\n";
      cout << "LOCAL_MIN_RC - Fatal error!\n";
      cout << "  A < B is required, but\n";
      cout << "  A = " << a << "\n";
      cout << "  B = " << b << "\n";
      status = -1;
      exit ( 1 );
    }
    c = 0.5 * ( 3.0 - qSqrt ( 5.0 ) );

    v = a + c * ( b - a );
    w = v;
    x = v;
    e = 0.0;

    status = 1;
    arg = x;

    return arg;
  }
//
//  STATUS (INPUT) = 1, return with initial function value of FX.
//
  else if ( status == 1 )
  {
    fx = value;
    fv = fx;
    fw = fx;
  }
//
//  STATUS (INPUT) = 2 or more, update the data.
//
  else if ( 2 <= status )
  {
    fu = value;

    if ( fu <= fx )
    {
      if ( x <= u )
      {
        a = x;
      }
      else
      {
        b = x;
      }
      v = w;
      fv = fw;
      w = x;
      fw = fx;
      x = u;
      fx = fu;
    }
    else
    {
      if ( u < x )
      {
        a = u;
      }
      else
      {
        b = u;
      }

      if ( fu <= fw || w == x )
      {
        v = w;
        fv = fw;
        w = u;
        fw = fu;
      }
      else if ( fu <= fv || v == x || v == w )
      {
        v = u;
        fv = fu;
      }
    }
  }
//
//  Take the next step.
//
  midpoint = 0.5 * ( a + b );
  tol1 = sqrt_eps * std::abs ( x ) + eps / 3.0;
  tol2 = 2.0 * tol1;
//
//  If the stopping criterion is satisfied, we can exit.
//
  if ( std::abs ( x - midpoint ) <= ( tol2 - 0.5 * ( b - a ) ) )
  {
    status = 0;
    return arg;
  }
//
//  Is golden-section necessary?
//
  if ( std::abs ( e ) <= tol1 )
  {
    if ( midpoint <= x )
    {
      e = a - x;
    }
    else
    {
      e = b - x;
    }
    d = c * e;
  }
//
//  Consider fitting a parabola.
//
  else
  {
    r = ( x - w ) * ( fx - fv );
    q = ( x - v ) * ( fx - fw );
    p = ( x - v ) * q - ( x - w ) * r;
    q = 2.0 * ( q - r );
    if ( 0.0 < q )
    {
      p = - p;
    }
    q = std::abs ( q );
    r = e;
    e = d;
//
//  Choose a golden-section step if the parabola is not advised.
//
    if (
      ( std::abs ( 0.5 * q * r ) <= std::abs ( p ) ) ||
      ( p <= q * ( a - x ) ) ||
      ( q * ( b - x ) <= p ) )
    {
      if ( midpoint <= x )
      {
        e = a - x;
      }
      else
      {
        e = b - x;
      }
      d = c * e;
    }
//
//  Choose a parabolic interpolation step.
//
    else
    {
      d = p / q;
      u = x + d;

      if ( ( u - a ) < tol2 )
      {
        d = tol1 * sign ( midpoint - x );
      }

      if ( ( b - u ) < tol2 )
      {
        d = tol1 * sign ( midpoint - x );
      }
    }
  }
//
//  F must not be evaluated too close to X.
//
  if ( tol1 <= std::abs ( d ) )
  {
    u = x + d;
  }
  if ( std::abs ( d ) < tol1 )
  {
    u = x + tol1 * sign ( d );
  }
//
//  Request value of F(U).
//
  arg = u;
  status = status + 1;

  return arg;
}
//****************************************************************************80

qreal Brent::zero ( qreal a, qreal b, qreal t, func_base& f )

//****************************************************************************80
//
//  Purpose:
//
//    ZERO seeks the root of a function F(X) in an interval [A,B].
//
//  Discussion:
//
//    The interval [A,B] must be a change of sign interval for F.
//    That is, F(A) and F(B) must be of opposite signs.  Then
//    assuming that F is continuous implies the existence of at least
//    one value C between A and B for which F(C) = 0.
//
//    The location of the zero is determined to within an accuracy
//    of 6 * MACHEPS * std::abs ( C ) + 2 * T.
//
//    Thanks to Thomas Secretin for pointing out a transcription error in the
//    setting of the value of P, 11 February 2013.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    11 February 2013
//
//  Author:
//
//    Original FORTRAN77 version by Richard Brent.
//    C++ version by John Burkardt.
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//  Parameters:
//
//    Input, qreal A, B, the endpoints of the change of sign interval.
//
//    Input, qreal T, a positive error tolerance.
//
//    Input, func_base& F, the name of a user-supplied c++ functor
//    whose zero is being sought.  The input and output
//    of F() are of type qreal.
//
//    Output, qreal ZERO, the estimated value of a zero of
//    the function F.
//
{
  qreal c;
  qreal d;
  qreal e;
  qreal fa;
  qreal fb;
  qreal fc;
  qreal m;
  qreal p;
  qreal q;
  qreal r;
  qreal s;
  qreal sa;
  qreal sb;
  qreal tol;
//
//  Make local copies of A and B.
//
  sa = a;
  sb = b;
  fa = f ( sa );
  fb = f ( sb );

  c = sa;
  fc = fa;
  e = sb - sa;
  d = e;

  for ( ; ; )
  {
    if ( std::abs ( fc ) < std::abs ( fb ) )
    {
      sa = sb;
      sb = c;
      c = sa;
      fa = fb;
      fb = fc;
      fc = fa;
    }

    tol = 2.0 * eps * std::abs ( sb ) + t;
    m = 0.5 * ( c - sb );

    if ( std::abs ( m ) <= tol || fb == 0.0 )
    {
      break;
    }

    if ( std::abs ( e ) < tol || std::abs ( fa ) <= std::abs ( fb ) )
    {
      e = m;
      d = e;
    }
    else
    {
      s = fb / fa;

      if ( sa == c )
      {
        p = 2.0 * m * s;
        q = 1.0 - s;
      }
      else
      {
        q = fa / fc;
        r = fb / fc;
        p = s * ( 2.0 * m * q * ( q - r ) - ( sb - sa ) * ( r - 1.0 ) );
        q = ( q - 1.0 ) * ( r - 1.0 ) * ( s - 1.0 );
      }

      if ( 0.0 < p )
      {
        q = - q;
      }
      else
      {
        p = - p;
      }

      s = e;
      e = d;

      if ( 2.0 * p < 3.0 * m * q - std::abs ( tol * q ) &&
        p < std::abs ( 0.5 * s * q ) )
      {
        d = p / q;
      }
      else
      {
        e = m;
        d = e;
      }
    }
    sa = sb;
    fa = fb;

    if ( tol < std::abs ( d ) )
    {
      sb = sb + d;
    }
    else if ( 0.0 < m )
    {
      sb = sb + tol;
    }
    else
    {
      sb = sb - tol;
    }

    fb = f ( sb );

    if ( ( 0.0 < fb && 0.0 < fc ) || ( fb <= 0.0 && fc <= 0.0 ) )
    {
      c = sa;
      fc = fa;
      e = sb - sa;
      d = e;
    }
  }
  return sb;
}
//****************************************************************************80

void Brent::zero_rc ( qreal a, qreal b, qreal t, qreal &arg, int &status,
  qreal value )

//****************************************************************************80
//
//  Purpose:
//
//    ZERO_RC seeks the root of a function F(X) using reverse communication.
//
//  Discussion:
//
//    The interval [A,B] must be a change of sign interval for F.
//    That is, F(A) and F(B) must be of opposite signs.  Then
//    assuming that F is continuous implies the existence of at least
//    one value C between A and B for which F(C) = 0.
//
//    The location of the zero is determined to within an accuracy
//    of 6 * MACHEPS * std::abs ( C ) + 2 * T.
//
//    The routine is a revised version of the Brent zero finder
//    algorithm, using reverse communication.
//
//    Thanks to Thomas Secretin for pointing out a transcription error in the
//    setting of the value of P, 11 February 2013.
//
//  Licensing:
//
//    This code is distributed under the GNU LGPL license.
//
//  Modified:
//
//    11 February 2013
//
//  Author:
//
//    John Burkardt
//
//  Reference:
//
//    Richard Brent,
//    Algorithms for Minimization Without Derivatives,
//    Dover, 2002,
//    ISBN: 0-486-41998-3,
//    LC: QA402.5.B74.
//
//  Parameters:
//
//    Input, qreal A, B, the endpoints of the change of sign interval.
//
//    Input, qreal T, a positive error tolerance.
//
//    Output, qreal &ARG, the currently considered point.  The user
//    does not need to initialize this value.  On return with STATUS positive,
//    the user is requested to evaluate the function at ARG, and return
//    the value in VALUE.  On return with STATUS zero, ARG is the routine's
//    estimate for the function's zero.
//
//    Input/output, int &STATUS, used to communicate between
//    the user and the routine.  The user only sets STATUS to zero on the first
//    call, to indicate that this is a startup call.  The routine returns STATUS
//    positive to request that the function be evaluated at ARG, or returns
//    STATUS as 0, to indicate that the iteration is complete and that
//    ARG is the estimated zero
//
//    Input, qreal VALUE, the function value at ARG, as requested
//    by the routine on the previous call.
//
{
  static qreal c;
  static qreal d;
  static qreal e;
  static qreal fa;
  static qreal fb;
  static qreal fc;
  qreal m;
  qreal p;
  qreal q;
  qreal r;
  qreal s;
  static qreal sa;
  static qreal sb;
  qreal tol;
//
//  Input STATUS = 0.
//  Initialize, request F(A).
//
  if ( status == 0 )
  {
    sa = a;
    sb = b;
    e = sb - sa;
    d = e;

    status = 1;
    arg = a;
    return;
  }
//
//  Input STATUS = 1.
//  Receive F(A), request F(B).
//
  else if ( status == 1 )
  {
    fa = value;
    status = 2;
    arg = sb;
    return;
  }
//
//  Input STATUS = 2
//  Receive F(B).
//
  else if ( status == 2 )
  {
    fb = value;

    if ( 0.0 < fa * fb )
    {
      status = -1;
      return;
    }
    c = sa;
    fc = fa;
  }
  else
  {
    fb = value;

    if ( ( 0.0 < fb && 0.0 < fc ) || ( fb <= 0.0 && fc <= 0.0 ) )
    {
      c = sa;
      fc = fa;
      e = sb - sa;
      d = e;
    }
  }
//
//  Compute the next point at which a function value is requested.
//
  if ( std::abs ( fc ) < std::abs ( fb ) )
  {
    sa = sb;
    sb = c;
    c = sa;
    fa = fb;
    fb = fc;
    fc = fa;
  }

  tol = 2.0 * eps * std::abs ( sb ) + t;
  m = 0.5 * ( c - sb );

  if ( std::abs ( m ) <= tol || fb == 0.0 )
  {
    status = 0;
    arg = sb;
    return;
  }

  if ( std::abs ( e ) < tol || std::abs ( fa ) <= std::abs ( fb ) )
  {
    e = m;
    d = e;
  }
  else
  {
    s = fb / fa;

    if ( sa == c )
    {
      p = 2.0 * m * s;
      q = 1.0 - s;
    }
    else
    {
      q = fa / fc;
      r = fb / fc;
      p = s * ( 2.0 * m * q * ( q - r ) - ( sb - sa ) * ( r - 1.0 ) );
      q = ( q - 1.0 ) * ( r - 1.0 ) * ( s - 1.0 );
    }

    if ( 0.0 < p )
    {
      q = - q;
    }
    else
    {
      p = - p;
    }
    s = e;
    e = d;

    if ( 2.0 * p < 3.0 * m * q - std::abs ( tol * q ) &&
         p < std::abs ( 0.5 * s * q ) )
    {
      d = p / q;
    }
    else
    {
      e = m;
      d = e;
    }
  }

  sa = sb;
  fa = fb;

  if ( tol < std::abs ( d ) )
  {
    sb = sb + d;
  }
  else if ( 0.0 < m )
  {
    sb = sb + tol;
  }
  else
  {
    sb = sb - tol;
  }

  arg = sb;
  status = status + 1;

  return;
}

// ======================================================================
// === Simple wrapper functions
// === for convenience and/or compatibility.
//
// === The three functions are the same as above,
// === except that they take a plain function F
// === instead of a c++ functor.  In all cases, the
// === input and output of F() are of type qreal.

typedef qreal qrealOfqreal (qreal);

class func_wrapper : public func_base {
  qrealOfqreal* func;
public:
  func_wrapper(qrealOfqreal* f) {
    func = f;
  }
  virtual qreal operator() (qreal x){
    return func(x);
  }
};

//****************************************************************************80

qreal Brent::glomin ( qreal a, qreal b, qreal c, qreal m, qreal e,
         qreal t, qreal f ( qreal x ), qreal &x ){
  func_wrapper foo(f);
  return glomin(a, b, c, m, e, t, foo, x);
}

//****************************************************************************80

qreal Brent::local_min ( qreal a, qreal b, qreal t, qreal f ( qreal x ),
  qreal &x ){
  func_wrapper foo(f);
  return local_min(a, b, t, foo, x);
}

//****************************************************************************80

qreal Brent::zero ( qreal a, qreal b, qreal t, qreal f ( qreal x ) ){
  func_wrapper foo(f);
  return zero(a, b, t, foo);
}

// ======================================================================
// Generally useful functor to evaluate a monic polynomial.
// For details, see class definition in brent.hpp

qreal monicPoly::operator()(qreal x){
  qreal rslt(1);
  for (int ii = coeff.size()-1; ii >= 0; ii--){
    rslt *= x;
    rslt += coeff[ii];
  }
  return rslt;
}

// Similarly, evaluate a general polynomial (not necessarily monic):
qreal Poly::operator()(qreal x){
  qreal rslt(0);
  for (int ii = coeff.size()-1; ii >= 0; ii--){
    rslt *= x;
    rslt += coeff[ii];
  }
  return rslt;
}

} // end namespace brent
