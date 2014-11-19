/****************************************************************************

 This code is distributed under the GNU LGPL license.

****************************************************************************/

#ifndef SPLINE_H
#define SPLINE_H

#include <QtGlobal>
#include <string>

namespace hrlib {

qreal basis_function_b_val ( qreal tdata[], qreal tval );
qreal basis_function_beta_val ( qreal beta1, qreal beta2, qreal tdata[],
  qreal tval );
qreal *basis_matrix_b_uni ( );
qreal *basis_matrix_beta_uni ( qreal beta1, qreal beta2 );
qreal *basis_matrix_bezier ( );
qreal *basis_matrix_hermite ( );
qreal *basis_matrix_overhauser_nonuni ( qreal alpha, qreal beta );
qreal *basis_matrix_overhauser_nul ( qreal alpha );
qreal *basis_matrix_overhauser_nur ( qreal beta );
qreal *basis_matrix_overhauser_uni ( void);
qreal *basis_matrix_overhauser_uni_l ( );
qreal *basis_matrix_overhauser_uni_r ( );
qreal basis_matrix_tmp ( int left, int n, qreal mbasis[], int ndata,
  qreal tdata[], qreal ydata[], qreal tval );
void bc_val ( int n, qreal t, qreal xcon[], qreal ycon[], qreal *xval,
  qreal *yval );
qreal bez_val ( int n, qreal x, qreal a, qreal b, qreal y[] );
qreal bpab_approx ( int n, qreal a, qreal b, qreal ydata[], qreal xval );
qreal *bp01 ( int n, qreal x );
qreal *bpab ( int n, qreal a, qreal b, qreal x );
int chfev ( qreal x1, qreal x2, qreal f1, qreal f2, qreal d1, qreal d2,
  int ne, qreal xe[], qreal fe[], int next[] );
int d3_fs ( qreal a1[], qreal a2[], qreal a3[], int n, qreal b[], qreal x[] );
qreal *d3_mxv ( int n, qreal a[], qreal x[] );
qreal *d3_np_fs ( int n, qreal a[], qreal b[] );
void d3_print ( int n, qreal a[], std::string title );
void d3_print_some ( int n, qreal a[], int ilo, int jlo, int ihi, int jhi );
qreal *d3_uniform ( int n, int *seed );
void data_to_dif ( int ntab, qreal xtab[], qreal ytab[], qreal diftab[] );
qreal dif_val ( int ntab, qreal xtab[], qreal diftab[], qreal xval );
int i4_max ( int i1, int i2 );
int i4_min ( int i1, int i2 );
void least_set ( int point_num, qreal x[], qreal f[], qreal w[],
  int nterms, qreal b[], qreal c[], qreal d[] );
qreal least_val ( int nterms, qreal b[], qreal c[], qreal d[],
  qreal x );
void least_val2 ( int nterms, qreal b[], qreal c[], qreal d[], qreal x,
  qreal *px, qreal *pxp );
void least_set_old ( int ntab, qreal xtab[], qreal ytab[], int ndeg,
  qreal ptab[], qreal b[], qreal c[], qreal d[], qreal *eps, int *ierror );
qreal least_val_old ( qreal x, int ndeg, qreal b[], qreal c[], qreal d[] );
void parabola_val2 ( int ndim, int ndata, qreal tdata[], qreal ydata[],
  int left, qreal tval, qreal yval[] );
qreal pchst ( qreal arg1, qreal arg2 );
qreal r8_uniform_01 ( int *seed );
void r8vec_bracket ( int n, qreal x[], qreal xval, int *left, int *right );
void r8vec_bracket3 ( int n, qreal t[], qreal tval, int *left );
qreal *r8vec_even_new ( int n, qreal alo, qreal ahi );
qreal *r8vec_indicator_new ( int n );
int r8vec_order_type ( int n, qreal x[] );
void r8vec_print ( int n, qreal a[], std::string title );
void r8vec_sort_bubble_a ( int n, qreal a[] );
qreal *r8vec_uniform_new ( int n, qreal b, qreal c, int *seed );
int r8vec_unique_count ( int n, qreal a[], qreal tol );
void r8vec_zero ( int n, qreal a[] );
qreal spline_b_val ( int ndata, qreal tdata[], qreal ydata[], qreal tval );
qreal spline_beta_val ( qreal beta1, qreal beta2, int ndata, qreal tdata[],
  qreal ydata[], qreal tval );
qreal spline_constant_val ( int ndata, qreal tdata[], qreal ydata[], qreal tval );
qreal *spline_cubic_set ( int n, qreal t[], qreal y[], int ibcbeg, qreal ybcbeg,
  int ibcend, qreal ybcend );
qreal spline_cubic_val ( int n, qreal t[], qreal y[], qreal ypp[],
  qreal tval, qreal *ypval, qreal *yppval );
void spline_cubic_val2 ( int n, qreal t[], qreal tval, int *left, qreal y[],
  qreal ypp[], qreal *yval, qreal *ypval, qreal *yppval );
qreal *spline_hermite_set ( int ndata, qreal tdata[], qreal ydata[],
  qreal ypdata[] );
void spline_hermite_val ( int ndata, qreal tdata[], qreal c[], qreal tval,
  qreal *sval, qreal *spval );
qreal spline_linear_int ( int ndata, qreal tdata[], qreal ydata[], qreal a,
  qreal b );
void spline_linear_intset ( int int_n, qreal int_x[], qreal int_v[],
  qreal data_x[], qreal data_y[] );
void spline_linear_val ( int ndata, qreal tdata[], qreal ydata[],
  qreal tval, qreal *yval, qreal *ypval );
qreal spline_overhauser_nonuni_val ( int ndata, qreal tdata[],
  qreal ydata[], qreal tval );
qreal spline_overhauser_uni_val ( int ndata, qreal tdata[], qreal ydata[],
  qreal tval );
void spline_overhauser_val ( int ndim, int ndata, qreal tdata[], qreal ydata[],
  qreal tval, qreal yval[] );
void spline_pchip_set ( int n, qreal x[], qreal f[], qreal d[] );
void spline_pchip_val ( int n, qreal x[], qreal f[], qreal d[], int ne,
  qreal xe[], qreal fe[] );
void spline_quadratic_val ( int ndata, qreal tdata[], qreal ydata[],
  qreal tval, qreal *yval, qreal *ypval );

}

#endif // SPLINE_H
