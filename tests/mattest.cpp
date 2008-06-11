
#include <vb/Matrix.h>
#include <vb/Real.h>
#include <vb/PRNG.h>
#include <iostream>
#include <math.h>

using namespace std;
using namespace vb;

#define DIM 200

//namespace vb { typedef double Real; }

Real f (Vector<Real> x) {
  Real o = DIM;
  for (unsigned int i=0; i<DIM; ++i) o -= cos(x[i]);
  return o;
}

Vector<Real> g (Vector<Real> x) {
  Vector<Real> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]);
  return out;
}

#define m1 .3
#define m2 .8

/** Line-search as a plug-in for a numerical optimization algorithm.
 *
 * It implements Wolfe's method. Reference : J.F. Bonnans et al., 
 * "Numerical Optimization" (2ed, Springer, 2006), p. 43.
 *
 * @todo Improvr the choice of the new point using polynomial 
 * interpolation instead of linear ? Not sure whether it actually 
 * improves speed of convergence, but needs to be tried.
 */

Real line_search (Real f (Vector<Real>), Vector<Real> g (Vector<Real>), Vector<Real> x, Vector<Real> d) {
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
  return t;
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

Vector<Real> minimize (Real f (Vector<Real>), Vector<Real> g (Vector<Real>), Vector<Real> x0) {
  Vector<Real>  x = x0;
  Real         ff = f(x);
  Vector<Real> gg = g(x);

  Matrix<Real> W(DIM,DIM);
  for (int i=0; i<DIM; ++i) W.data[i][i] = 1.0;

  Vector<Real> ss(DIM);
  Real newff,ys,u;
  Vector<Real> newgg(DIM);
  Vector<Real> yy(DIM);
  Vector<Real> WW(DIM);

  for (int i=0;;++i) {
    // cerr << ".";

    ss     = -(W*gg);
    ss    *= line_search(f,g,x,ss);
    x     += ss;
    newff  = f(x);

    cerr << newff << endl;

    if (newff >= ff) { x -= ss; break; }

    newgg = g(x);
    yy    = newgg - gg;
    ys    = scalar_product(yy,ss);
    WW    = W*(yy/ys);
    u     = 1.0 + scalar_product(yy,WW);
    W    += aTb((u/ys)*ss-WW,ss);
    W    -= aTb(ss,WW);

    ff    = newff;
    gg    = newgg;
  }

  return x;
}

int main () {
  Vector<Real> x(DIM);
  for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  x = minimize (f,g,x);
  cout << "Final value: " << x << " -> " << f(x) << endl;
}
