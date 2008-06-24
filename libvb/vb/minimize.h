
/// @file minimize.h
/// Algorithms for function minimization.

#ifndef __VB_MINIMIZE_H
#define __VB_MINIMIZE_H

#define DBG std::cerr << __FILE__ << " " << __LINE__ << std::endl

#include <vb/Matrix.h>

namespace vb {
  template <class T> class Minimizer {
    public:
      T (*f) (const Vector<T> &, void *);
      Vector<T> (*g) (const Vector<T> &, void *);
      void *context;

      Vector<T> x; T fx; Vector<T> gx;
      Vector<T> old_x; T old_fx; Vector<T> old_gx;

      T compute (const Vector<T> &x_) {
        x = x_;
        fx = f(x,context);
        gx = g(x,context);
      }

      Minimizer (T f_ (const Vector<T> &, void *), Vector<T> g_ (const Vector<T> &, void *), void *context_ = NULL) :
        f(f_), g(g_), context(context_) { };

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

      void line_search (const Vector<T> d) {
        old_x=x; old_fx=fx; old_gx=gx;
        T qq_0 = .8 * scalar_product (gx,d);
        T dir = (qq_0>0 ? -1 : 1);
        T t_l = 0.0, t_r = 0.0, t = dir;
        T y;
        bool refining = false;

        while (true) {
          compute (old_x + t*d);
          y = old_fx + .3 * t * qq_0;

          if ((fx<=y) && (dir*scalar_product (gx,d) >= dir*qq_0)) break;
          if (fx>y) { t_r=t; refining = true; } else t_l = t;
          if (refining) t = (t_r+t_l)/2.0; else t *= 2.0;
          if (t-t_l+1.0 == 1.0) break;
        }
      }

      /** Function minimization by a steepest-descent algorithm.
       *
       * It has been called a "numerical absurdity", but it still works 
       * pretty well compared to BFGS especially in very high dimension 
       * (when storage of the approximate inverse Hessian is problematic).
       */

      T minimize_grad (const Vector<T> &x0) {
        compute(x0);
        old_x  = x;
        old_fx = fx+1;
        old_gx = gx;

        while (fx < old_fx) {
          line_search (gx);
        }
        return fx;
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

      T minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0 = Vector<T>(0)) {
        compute (x0);

        old_x  = x;
        old_fx = fx+1;
        old_gx = gx;

        Vector<T> dx,dg,Wdg;
        T dgdx,u;

        Vector<T> diag = W0;
        if (diag.size() == 0) diag = Vector<T> (x0.size(), T(1.0));
        Matrix<T> W(x0.size(),x0.size(),diag);

        while (fx < old_fx) {
          line_search(W*gx);

          dx = x - old_x;
          dg = gx - old_gx;
          Wdg = W*dg;
          dgdx = scalar_product(dg,dx);
          dx /= dgdx;
          u = dgdx + scalar_product(dg,Wdg);

          W.rank1update(u*dx-Wdg,dx);
          W.rank1update(dx,-Wdg);

        }

        return fx;
      }
  };
}

#endif
