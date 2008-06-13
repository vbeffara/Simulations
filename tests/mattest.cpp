
#include <vb/Matrix.h>
//#include <vb/Real.h>
#include <vb/PRNG.h>
#include <iostream>
#include <math.h>

#define Real double

using namespace std;
using namespace vb;

#define DIM 400

//namespace vb { typedef double Real; }

Real f (Vector<Real> x) {
  Real o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<Real> g (Vector<Real> x) {
  Vector<Real> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

#define m1 .3
#define m2 .8

typedef pair <Vector<Real>,Real> PointValue;

/** Line-search as a plug-in for a numerical optimization algorithm.
 *
 * It implements Wolfe's method. Reference : J.F. Bonnans et al., 
 * "Numerical Optimization" (2ed, Springer, 2006), p. 43.
 *
 * @todo Improvr the choice of the new point using polynomial 
 * interpolation instead of linear ? Not sure whether it actually 
 * improves speed of convergence, but needs to be tried.
 */

PointValue line_search (Real f (Vector<Real>), Vector<Real> g (Vector<Real>), Vector<Real> x, Vector<Real> d) {
  bool reverse = false;
  if (scalar_product(g(x),d)>0) { reverse = true; d = -d; }

  Real t_l = 0.0, t_r = 0.0, t = 1.0;
  Real q_0 = f(x);
  Real qq_0 = m2 * scalar_product (g(x),d);
  Real q, qq, y;
  Vector<Real> xx=x;
  bool refining = false;

  while (true) {
    xx=x+t*d;
    q = f(xx);
    qq = scalar_product (g(xx),d);
    y = q_0 + m1 * t * qq_0;

    if ((q<=y) && (qq>=qq_0)) break;
    if (q>y) { t_r=t; refining = true; } else t_l = t;
    if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
    if (t-t_l+1.0 == 1.0) break;
  }

  if (reverse) { t = -t; }
  return PointValue(xx,q);
}

/** A quasi-Newtonian minimization algorithm.
 *
 * It is the algorithm of Broyden, Fletcher, Goldfarb and Shanno (BFGS 
 * method).  Reference : J.F. Bonnans et al., "Numerical Optimization" 
 * (2ed, Springer, 2006), p. 54.
 *
 * In dimension N it has to maintain an N by N matrix, which limits it 
 * to a few thousand dimensions.
 *
 * The iteration step attempts to minimize the number of vb::Real 
 * multiplications while remaining readable. Maybe epsilon more 
 * optimization is doable, but it is quite efficient already.
 *
 * @todo Find something that works in higher dimension, for circle 
 * packings and such: this one is enough for small figures, but not for 
 * scaling limits. Maybe using the fact that W-I has small rank (twice 
 * the number of iterates at most) and hoping that the number of 
 * iterates remains smaller that the dimension, which does seem to be 
 * the case, and templating it.
 *
 * @todo Put it inside libvb (vb/Numerical.h) and actually use it for 
 * circle packings.
 */

Vector<Real> minimize_bfgs (Real f (Vector<Real>), Vector<Real> g (Vector<Real>), Vector<Real> x0, Vector<Real> W0 = Vector<Real>(0)) {
  PointValue cur_pv (x0,f(x0));
  PointValue old_pv (x0,cur_pv.second+1);

  Vector<Real> cur_g = g(cur_pv.first);
  Vector<Real> old_g = cur_g;

  Vector<Real> dx,dg,Wdg;
  Real dgdx,u;

  if (W0.size() == 0) W0 = Vector<Real> (x0.size(), Real(1.0));
  Matrix<Real> W(DIM,DIM,W0);

  while (cur_pv.second < old_pv.second) {
    old_pv = cur_pv; cur_pv = line_search(f,g,cur_pv.first, -(W*cur_g));
    old_g  = cur_g;  cur_g  = g(cur_pv.first);

    dx = cur_pv.first - old_pv.first;
    dg = cur_g - old_g;
    Wdg = W*dg;
    dgdx = scalar_product(dg,dx);
    dx /= dgdx;
    u = dgdx + scalar_product(dg,Wdg);

    W.rank1update(u*dx-Wdg,dx);
    W.rank1update(dx,-Wdg);
  }

  return old_pv.first;
}

Vector<Real> minimize_grad (Real f (Vector<Real>), Vector<Real> g (Vector<Real>), Vector<Real> x0) {
  Vector<Real> x=x0;
  Real ff=f(x), old_ff=ff+1;
  Vector<Real> gg;

  while (true) {
    gg = g(x);
    x = line_search(f,g,x,gg).first;

    old_ff = ff;
    ff = f(x);
    if (ff >= old_ff) { x -= gg; break; }
  }
  return x;
}

int main () {
  //Vector<double> V(8); for (int i=0; i<3; ++i) V[i]=i+1;
  //Matrix<double> M(8,8);
  //M.rank1update(V,V);
  //cout << M << endl;

  Vector<Real> x(DIM); for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  Vector<Real> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);
  x = minimize_bfgs (f,g,x);
  cout << "Final value: " << f(x) << endl;
}
