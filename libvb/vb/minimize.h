
/// @file minimize.h
/// Algorithms for function minimization.

#ifndef __VB_MINIMIZE_H
#define __VB_MINIMIZE_H

#define DBG std::cerr << __FILE__ << " " << __LINE__ << std::endl

#include <vb/Matrix.h>

namespace vb {
  /** A utility class for function minimization.
   *
   * It stores the triple of a point, a value and a gradient.
   */

  template <class T> class PointValueGradient {
    public:
      Vector<T> point;     ///< The position of the point.
      T value;             ///< The value of a function at the point.
      Vector<T> gradient;  ///< The gradient of a function at the point.

      /// Constructor from the three constituents.

      PointValueGradient (const Vector<T> &p, const T &v, const Vector<T> &g) : point(p), value(v), gradient(g) { };
  };

  /** Line-search as a plug-in for a numerical optimization algorithm.
   *
   * It implements Wolfe's method. Reference : J.F. Bonnans et al., 
   * "Numerical Optimization" (2ed, Springer, 2006), p. 43.
   *
   * Parameters chosen : m1 = 0.3, m2 = 0.8.
   *
   * @todo Improve the choice of the new point using polynomial 
   * interpolation instead of linear ? Not sure whether it actually 
   * improves speed of convergence, but needs to be tried.
   */

  template <class T> PointValueGradient<T> line_search (
      T f (const Vector<T> &, void*),
      Vector<T> g (const Vector<T> &, void*),
      const Vector<T> &x,
      const Vector<T> &d,
      void *context = NULL)
  {
    bool reverse = false;
    Vector<T> dd = d;
    if (scalar_product(g(x,context),d)>0) { reverse = true; dd = -dd; }

    T t_l = 0.0, t_r = 0.0, t = 1.0;
    T q_0 = f(x,context);
    T qq_0 = .8 * scalar_product (g(x,context),dd);
    T q, qq, y;
    Vector<T> xx=x;
    Vector<T> gg;
    bool refining = false;

    while (true) {
      xx=x+t*dd;
      q = f(xx,context);
      gg = g(xx,context);
      qq = scalar_product (gg,dd);
      y = q_0 + .3 * t * qq_0;

      if ((q<=y) && (qq>=qq_0)) break;
      if (q>y) { t_r=t; refining = true; } else t_l = t;
      if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
      if (t-t_l+1.0 == 1.0) break;
    }

    if (reverse) { t = -t; }
    return PointValueGradient<T> (xx,q,gg);
  }

  /** A quasi-Newtonian minimization algorithm.
   *
   * It is the algorithm of Broyden, Fletcher, Goldfarb and Shanno (BFGS 
   * method).  Reference : J.F. Bonnans et al., "Numerical Optimization" 
   * (2ed, Springer, 2006), p. 54.
   *
   * In dimension N it has to maintain an N by N matrix, which limits it 
   * to a few thousand dimensions.
   */

  template <class T> PointValueGradient<T> minimize_bfgs (
      T f (const Vector<T> &, void*),
      Vector<T> g (const Vector<T> &, void*),
      const Vector<T> &x0,
      void *context = NULL,
      const Vector<T> &W0 = Vector<T>(0))
  {
    PointValueGradient<T> cur_pvg (x0,f(x0,context),g(x0,context));
    PointValueGradient<T> old_pvg (x0,cur_pvg.value+1,cur_pvg.gradient);

    Vector<T> dx,dg,Wdg;
    T dgdx,u;

    Vector<T> diag = W0;
    if (diag.size() == 0) diag = Vector<T> (x0.size(), T(1.0));
    Matrix<T> W(x0.size(),x0.size(),diag);

    while (cur_pvg.value < old_pvg.value) {
      old_pvg = cur_pvg; cur_pvg = line_search(f,g,old_pvg.point, -(W*old_pvg.gradient), context);

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

  /** Function minimization by a steepest-descent algorithm.
   *
   * It has been called a "numerical absurdity", but it still works 
   * pretty well compared to BFGS especially in very high dimension 
   * (when storage of the approximate inverse Hessian is problematic).
   */

  template <class T> PointValueGradient<T> minimize_grad (
      T f (const Vector<T> &, void*),
      Vector<T> g (const Vector<T> &, void*),
      const Vector<T> &x0,
      void *context = NULL)
  {
    PointValueGradient<T> cur_pvg (x0,f(x0,context),g(x0,context));
    PointValueGradient<T> old_pvg (x0,cur_pvg.value+1,cur_pvg.gradient);

    while (cur_pvg.value < old_pvg.value) {
      std::cerr << cur_pvg.value << std::endl;
      old_pvg = cur_pvg; cur_pvg = line_search (f,g,old_pvg.point,-old_pvg.gradient, context);
    }
    return old_pvg;
  }
}

#endif
