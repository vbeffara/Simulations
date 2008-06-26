
/// @file minimize.h
/// Algorithms for function minimization.

#ifndef __VB_MINIMIZE_H
#define __VB_MINIMIZE_H

#include <iostream>
#include <vb/Matrix.h>

namespace vb {

  /** A class to minimize functions.
   *
   * It implements various algorithms, and which one to choose heavily 
   * depends on the function ... See the separate documentation for each 
   * of them, they are named minimize_*.
   *
   * It is made as a template so that the algorithms can be used on 
   * double or vb::Real, the last one will eventually be using GMP.
   */

  template <typename T> class Minimizer {
    public:
      unsigned int n; ///< The dimension of the space.

      /** A pointer to the function to be minimized.
       *
       * That function should take a vb::Vector<T> as the point at which 
       * it is to be computed, and a void* representing the "context" in 
       * which it is defined (typically a pointer to an object 
       * containing additional information, as in the case of graph 
       * embeddings.
       *
       * If possible, better use the vb::Minimize::fg form (simultaneous 
       * computation of the function and its gradient), as it tends to 
       * be quicker.
       */

      T (*f) (const Vector<T> &, void *);
      
      /** A pointer to the gradient of the function.
       *
       * Same remarks as for vb::Minimizer::f.
       */
      
      Vector<T> (*g) (const Vector<T> &, void *);

      /** A pointer to joint computation of the function and its gradient.
       *
       * The function itself takes 3 arguments : a vb::Vector<T> as the 
       * point at which the function is to be computed, a reference to a 
       * vb::Vector<T> where it should store the computed gradient, and 
       * a void* pointing to the context of the computation, if any.
       *
       * This is the prefered way to specify the function to be 
       * minimized.
       */

      T (*fg) (const Vector<T> &, Vector<T> &, void *);

      /** The context of all f and g computations.
       *
       * It is expected to point to an object, and will be passed to all 
       * calls to (*f), (*g) and (*fg), to implement the "black box" 
       * principle: The optimization algorithms don't care about it and 
       * only focus on the value of x.
       *
       * It is set at construction time, though you can always change it 
       * later on (though why would you ... better to build another 
       * vb::Minimizer probably ...).
       */

      void *context;

      Vector<T> x;      ///< The current point of interest.
      T fx;             ///< The value of the function at x.
      Vector<T> gx;     ///< The gradient of the function at x.

      Vector<T> old_x;  ///< The previous value of x, before the last line_search().
      T old_fx;         ///< The value of the function at old_x.
      Vector<T> old_gx; ///< The gradient of the function at old_x.

      /** A pointer to a logging std::ostream.
       *
       * If not NULL, some of the algorithms will output some logging 
       * information there as they go, that is useful especially for 
       * long computations.
       */

      std::ostream *os;

      T er;    ///< An indicator of the current error, for logging.
      int ler; ///< The base-10 0logarithm of er.

      /** Compute the value of the function and its gradient.
       *
       * If fg is not NULL, it uses (*fg); if it is NULL, it will use 
       * (*f) and (*g) separately. Both work fine, but the first one is 
       * always faster.
       */

      T compute (const Vector<T> &x_);

      /** Perform a few initialization.
       *
       * In particular, it reserves space in x, gx, old_x and old_gx so 
       * that the algorithms can use swap() and in-place computations. 
       * Except that right now they don't.
       */

      void init ();

      /** The constructor for joint f and g computation.
       *
       * It calls init() after setting the functions. Logging and such 
       * need to be taken carre of separately.
       *
       * @param n_      The dimension of space.
       * @param fg_     The function to which vb::Minimizer::fg will point.
       * @param context The computation context that will be passed to * (*fg).
       */

      Minimizer (unsigned int n_,
                 T fg_ (const Vector<T> &, Vector<T> &, void *),
                 void *context_ = NULL) :
        n(n_), f(NULL), g(NULL), fg(fg_), context(context_) { init(); };

      /** The constructor for separate f and g computation.
       *
       * It calls init() after setting the functions. Logging and such 
       * need to be taken carre of separately.
       *
       * @param n_      The dimension of space.
       * @param f_      The function to which vb::Minimizer::f will point.
       * @param g_      The function to which vb::Minimizer::g will point.
       * @param context The computation context that will be passed to * (*fg).
       */

      Minimizer (unsigned int n_,
                 T f_ (const Vector<T> &, void *),
                 Vector<T> g_ (const Vector<T> &, void *),
                 void *context_ = NULL) :
        n(n_), f(f_), g(g_), fg(NULL), context(context_) { init(); };

      /** Line-search as a plug-in for a numerical optimization algorithm.
       *
       * It implements Wolfe's method. Reference : J.F. Bonnans et al., 
       * "Numerical Optimization" (2ed, Springer, 2006), p. 43.
       *
       * Parameters chosen : m1 = 0.3, m2 = 0.8.
       *
       * Careful if d is gx (for steepest gradient), because the 
       * algorith _will_ change the value of gx as it goes so it might 
       * lead to trouble ... That's why vb::Minimizer::minimize_grad 
       * calls it as line_search(Vector<T>(gx)).
       *
       * @todo Improve the choice of the new point using polynomial 
       * interpolation instead of linear ? Not sure whether it actually 
       * improves speed of convergence, but needs to be tried.
       *
       * @param d The direction of the search.
       */

      void line_search (const Vector<T> &d);

      /** Function minimization by a steepest-descent algorithm.
       *
       * It has been called a "numerical absurdity", but it still works 
       * pretty well compared to BFGS especially in very high dimension 
       * (when storage of the approximate inverse Hessian is problematic).
       *
       * @param x0 The point from which to start.
       */

      T minimize_grad (const Vector<T> &x0);

      /** A quasi-Newtonian minimization algorithm.
       *
       * It is the algorithm of Broyden, Fletcher, Goldfarb and Shanno (BFGS 
       * method).  Reference : J.F. Bonnans et al., "Numerical Optimization" 
       * (2ed, Springer, 2006), p. 54.
       *
       * In dimension N it has to maintain an N by N matrix, which limits it 
       * to a few thousand dimensions.
       *
       * @todo Change the prototype to accept a true vb::Matrix instead 
       * of a vector, and create a matrix storage type for diagonal 
       * matrices.
       *
       * @param x0 The point from which to start.
       * @param W0 The initial estimate for the inverse Hessian (as a * diagonal matrix).
       */

      T minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0 = Vector<T>(0));

      /** The Fletcher-Reeves conjugate gradient algorithm.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed, 
       * Springer, 2006), p. 73.
       *
       * @param x0 The point from which to start.
       */

      T minimize_fr (const Vector<T> &x0);

      /** The Pollak-Ribiere conjugate gradient algorithm.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed, 
       * Springer, 2006), p. 73.
       *
       * @param x0 The point from which to start.
       */

      T minimize_pr (const Vector<T> &x0);

      /** The mixed quasi-Newton / conjugate gradient method.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed, 
       * Springer, 2006), p. 74.
       *
       * If in doubt, choose this one !
       *
       * @param x0 The point from which to start.
       */

      T minimize_qn (const Vector<T> &x0);
  };

  /****************************************************/

  template <typename T> inline T Minimizer<T>::compute (const Vector<T> &x_) {
    x = x_;
    if (fg) {
      fx = fg (x,gx,context);
    } else {
      fx = f(x,context);
      gx = g(x,context);
    }
  }

  template <typename T> void Minimizer<T>::init () {
    os = NULL;
    er = 1.0;
    ler = 0;

    x = Vector<T> (n);
    gx = Vector<T> (n);
    old_x = Vector<T> (n);
    old_gx = Vector<T> (n);
  }

  template <typename T> void Minimizer<T>::line_search (const Vector<T> &d) {
    old_x.swap(x); old_fx=fx; old_gx.swap(gx);
    T qq_0 = .8 * scalar_product (old_gx,d);
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

    if (os) {
      T tmp = scalar_product (gx,gx);
      while (tmp<er) {
        er /= 10.0;
        ++ler;
        if (ler%10) *os << '.'; else *os << '*';
      }
    }
  }

  template <typename T> T Minimizer<T>::minimize_grad (const Vector<T> &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    while (fx < old_fx) {
      line_search (Vector<T>(gx));
    }
    return fx;
  }

  template <typename T> T Minimizer<T>::minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0) {
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

  template <typename T> T Minimizer<T>::minimize_fr (const Vector<T> &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector<T> d(n);
    Vector<T> old_d(n);
    bool first = true;

    while (fx < old_fx) {
      old_d = d; d = -gx;
      if (!first) {
        T c = scalar_product(gx,gx) / scalar_product(old_gx,old_gx);
        d += c * old_d;
      }
      line_search(d);
      first=false;
    }

    return fx;
  }

  template <typename T> T Minimizer<T>::minimize_pr (const Vector<T> &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector<T> d(n);
    Vector<T> old_d(n);
    bool first = true;

    while (fx < old_fx) {
      old_d = d; d = -gx;
      if (!first) {
        T c = scalar_product(gx - old_gx,gx) / scalar_product(old_gx,old_gx);
        d += c * old_d;
      }
      line_search(d);
      first=false;
    }

    return fx;
  }

  template <typename T> T Minimizer<T>::minimize_qn (const Vector<T> &x0) {
    compute(x0);
    old_x  = x;
    old_fx = fx+1;
    old_gx = gx;

    Vector<T> d(n);
    Vector<T> old_d(n);
    bool first = true;

    while (fx < old_fx) {
      old_d = d; d = -gx;
      if (!first) {
        Vector<T> y = gx - old_gx;
        T c = scalar_product(y,gx) / scalar_product(y,old_d);
        d += c * old_d;
      }
      line_search(d);
      first=false;
    }

    return fx;
  }
}

#endif
