/// @file
/// Algorithms for function minimization.

#ifndef __VB_MINIMIZER_H
#define __VB_MINIMIZER_H

#include <vb/Matrix.h>

namespace vb {
  /** A class to minimize functions.
   *
   * It implements various algorithms, and which one to choose heavily
   * depends on the function ... See the separate documentation for each
   * of them, they are named minimize_*.
   */

  class Minimizer {
    public:
      unsigned int n; ///< The dimension of the space.

      /** A pointer to the function to be minimized.
       *
       * That function should take a vb::Vector as the point at which
       * it is to be computed, and a void* representing the "context" in
       * which it is defined (typically a pointer to an object
       * containing additional information, as in the case of graph
       * embeddings.
       *
       * If possible, better use the vb::Minimize::fg form (simultaneous
       * computation of the function and its gradient), as it tends to
       * be quicker.
       */

      double (*f) (const Vector &, void *);

      /** A pointer to the gradient of the function.
       *
       * Same remarks as for vb::Minimizer::f.
       */

      Vector (*g) (const Vector &, void *);

      /** A pointer to joint computation of the function and its gradient.
       *
       * The function itself takes 3 arguments : a vb::Vector as the
       * point at which the function is to be computed, a reference to a
       * vb::Vector where it should store the computed gradient, and
       * a void* pointing to the context of the computation, if any.
       *
       * This is the prefered way to specify the function to be
       * minimized.
       */

      double (*fg) (const Vector &, Vector &, void *);

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

      Vector x;      ///< The current point of interest.
      double fx;     ///< The value of the function at x.
      Vector gx;     ///< The gradient of the function at x.

      Vector old_x;  ///< The previous value of x, before the last line_search().
      double old_fx; ///< The value of the function at old_x.
      Vector old_gx; ///< The gradient of the function at old_x.

      /** A pointer to a logging std::ostream.
       *
       * If not NULL, some of the algorithms will output some logging
       * information there as they go, that is useful especially for
       * long computations.
       */

      std::ostream *os;

      double er; ///< An indicator of the current error, for logging.
      int ler;   ///< The base-10 0logarithm of er.

      /** Compute the value of the function and its gradient.
       *
       * If fg is not NULL, it uses (*fg); if it is NULL, it will use
       * (*f) and (*g) separately. Both work fine, but the first one is
       * always faster.
       *
       * The default value (or an empty vector, or x itself) means that
       * x already contains the correct point coordinates.
       *
       * @param x_ The point at which the computation is to be done.
       * @return   The value of the function at x_.
       */

      double compute (const Vector &x_ = Vector(0));

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
       * @param n_       The dimension of space.
       * @param fg_      The function to which vb::Minimizer::fg will point.
       * @param context_ The computation context that will be passed to * (*fg).
       */

      Minimizer (unsigned int n_,
                 double fg_ (const Vector &, Vector &, void *),
                 void *context_ = NULL);

      /** The constructor for separate f and g computation.
       *
       * It calls init() after setting the functions. Logging and such
       * need to be taken carre of separately.
       *
       * @param n_       The dimension of space.
       * @param f_       The function to which vb::Minimizer::f will point.
       * @param g_       The function to which vb::Minimizer::g will point.
       * @param context_ The computation context that will be passed to * (*fg).
       */

      Minimizer (unsigned int n_,
                 double f_ (const Vector &, void *),
                 Vector g_ (const Vector &, void *),
                 void *context_ = NULL);

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
       * calls it as line_search(Vector(gx)).
       *
       * @todo Improve the choice of the new point using polynomial
       * interpolation instead of linear ? Not sure whether it actually
       * improves speed of convergence, but needs to be tried.
       *
       * @param d The direction of the search.
       */

      void line_search (const Vector &d);

      /** Function minimization by a steepest-descent algorithm.
       *
       * It has been called a "numerical absurdity", but it still works
       * pretty well compared to BFGS especially in very high dimension
       * (when storage of the approximate inverse Hessian is problematic).
       *
       * @param x0 The point from which to start.
       * @return   The value of the function at the end point.
       */

      double minimize_grad (const Vector &x0);

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
       * @return   The value of the function at the end point.
       */

      double minimize_bfgs (const Vector &x0, const Vector &W0 = Vector(0));

      /** The Fletcher-Reeves conjugate gradient algorithm.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed,
       * Springer, 2006), p. 73.
       *
       * @param x0 The point from which to start.
       * @return   The value of the function at the end point.
       */

      double minimize_fr (const Vector &x0);

      /** The Pollak-Ribiere conjugate gradient algorithm.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed,
       * Springer, 2006), p. 73.
       *
       * @param x0 The point from which to start.
       * @return   The value of the function at the end point.
       */

      double minimize_pr (const Vector &x0);

      /** The mixed quasi-Newton / conjugate gradient method.
       *
       * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed,
       * Springer, 2006), p. 74.
       *
       * If in doubt, choose this one !
       *
       * @param x0 The point from which to start.
       * @return   The value of the function at the end point.
       */

      double minimize_qn (const Vector &x0);
  };
}

#endif
