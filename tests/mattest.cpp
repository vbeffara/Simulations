
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

Real f (const Vector<Real> &x) {
  Real o = 0;
  for (unsigned int i=0; i<DIM; ++i) o += (1 - cos(x[i]/(i+1)));
  return o;
}

Vector<Real> g (const Vector<Real> &x) {
  Vector<Real> out(DIM);
  for (unsigned int i=0; i<DIM; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
  return out;
}

#define m1 .3
#define m2 .8

template <class T> class PointValueGradient {
  public:
    Vector<T> point;
    T value;
    Vector<T> gradient;

    PointValueGradient (const Vector<T> &p, const T &v, const Vector<T> &g) : point(p), value(v), gradient(g) { };
};

/** Line-search as a plug-in for a numerical optimization algorithm.
 *
 * It implements Wolfe's method. Reference : J.F. Bonnans et al., 
 * "Numerical Optimization" (2ed, Springer, 2006), p. 43.
 *
 * @todo Improvr the choice of the new point using polynomial 
 * interpolation instead of linear ? Not sure whether it actually 
 * improves speed of convergence, but needs to be tried.
 */

PointValueGradient<Real> line_search (Real f (const Vector<Real> &), Vector<Real> g (const Vector<Real> &), const Vector<Real> &x, const Vector<Real> &d) {
  bool reverse = false;
  Vector<Real> dd = d;
  if (scalar_product(g(x),d)>0) { reverse = true; dd = -dd; }

  Real t_l = 0.0, t_r = 0.0, t = 1.0;
  Real q_0 = f(x);
  Real qq_0 = m2 * scalar_product (g(x),dd);
  Real q, qq, y;
  Vector<Real> xx=x;
  Vector<Real> gg;
  bool refining = false;

  while (true) {
    xx=x+t*dd;
    q = f(xx);
    gg = g(xx);
    qq = scalar_product (gg,dd);
    y = q_0 + m1 * t * qq_0;

    if ((q<=y) && (qq>=qq_0)) break;
    if (q>y) { t_r=t; refining = true; } else t_l = t;
    if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
    if (t-t_l+1.0 == 1.0) break;
  }

  if (reverse) { t = -t; }
  return PointValueGradient<Real> (xx,q,gg);
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

PointValueGradient<Real> minimize_bfgs (Real f (const Vector<Real> &), Vector<Real> g (const Vector<Real> &), const Vector<Real> &x0, const Vector<Real> &W0 = Vector<Real>(0)) {
  PointValueGradient<Real> cur_pvg (x0,f(x0),g(x0));
  PointValueGradient<Real> old_pvg (x0,cur_pvg.value+1,cur_pvg.gradient);

  Vector<Real> dx,dg,Wdg;
  Real dgdx,u;

  Vector<Real> diag = W0;
  if (diag.size() == 0) diag = Vector<Real> (x0.size(), Real(1.0));
  Matrix<Real> W(DIM,DIM,diag);

  while (cur_pvg.value < old_pvg.value) {
    old_pvg = cur_pvg; cur_pvg = line_search(f,g,old_pvg.point, -(W*old_pvg.gradient));
    cerr << cur_pvg.value << endl;

    dx = cur_pvg.point - old_pvg.point;
    dg = cur_pvg.gradient - old_pvg.gradient;
    Wdg = W*dg;
    dgdx = scalar_product(dg,dx);
    dx /= dgdx;
    u = dgdx + scalar_product(dg,Wdg);

    W.rank1update(u*dx-Wdg,dx);
    W.rank1update(dx,-Wdg);
  }

  return old_pvg;
}

PointValueGradient<Real> minimize_grad (Real f (const Vector<Real> &), Vector<Real> g (const Vector<Real> &), const Vector<Real> &x0) {
  PointValueGradient<Real> cur_pvg (x0,f(x0),g(x0));
  PointValueGradient<Real> old_pvg (x0,cur_pvg.value+1,cur_pvg.gradient);

  while (cur_pvg.value < old_pvg.value) {
    old_pvg = cur_pvg; cur_pvg = line_search (f,g,old_pvg.point,-old_pvg.gradient);
  }
  return old_pvg;
}

int main () {
  //Vector<double> V(8); for (int i=0; i<3; ++i) V[i]=i+1;
  //Matrix<double> M(8,8);
  //M.rank1update(V,V);
  //cout << M << endl;

  Vector<Real> x(DIM); for (unsigned int i=0; i<DIM; ++i) x[i] = cos(i);
  Vector<Real> W0(DIM); for (unsigned int i=0; i<DIM; ++i) W0[i] = (i+1)*(i+1);
  PointValueGradient<Real> min = minimize_bfgs (f,g,x);
  cout << "Final value: " << min.value << endl;
}
