#pragma once /// @file
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace vb {
	template <typename T> using Vector = boost::numeric::ublas::vector<T>;

	template <typename T> class Minimizer { public:
		using Matrix = boost::numeric::ublas::matrix<T>;

		Minimizer (	unsigned int n_,
		           	T f_ (const Vector<T> &, void *),
		           	Vector<T> g_ (const Vector<T> &, void *),
		           	T fg_ (const Vector<T> &, Vector<T> &, void *),
		           	void *context_ = 0);

		Minimizer (	unsigned int n_,
		           	T fg_ (const Vector<T> &, Vector<T> &, void *),
		           	void *context_ = 0);

		Minimizer (	unsigned int n_,
		           	T f_ (const Vector<T> &, void *),
		           	Vector<T> g_ (const Vector<T> &, void *),
		           	void *context_ = 0);

		T compute (const Vector<T> & x = Vector<T>(0));

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
		 * @return   The value of the function at the end point.
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
		 * @return   The value of the function at the end point.
		 */

		T minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0 = Vector<T>(0));

		/** The Fletcher-Reeves conjugate gradient algorithm.
		 *
		 * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed,
		 * Springer, 2006), p. 73.
		 *
		 * @param x0 The point from which to start.
		 * @return   The value of the function at the end point.
		 */

		T minimize_fr (const Vector<T> &x0);

		/** The Pollak-Ribiere conjugate gradient algorithm.
		 *
		 * Reference: J.F. Bonnans et al., "Numerical Optimization" (2ed,
		 * Springer, 2006), p. 73.
		 *
		 * @param x0 The point from which to start.
		 * @return   The value of the function at the end point.
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
		 * @return   The value of the function at the end point.
		 */

		T minimize_qn (const Vector<T> &x0);

		unsigned n;

		T        	(*f) 	(const Vector<T> & x,                	void * context);
		Vector<T>	(*g) 	(const Vector<T> & x,                	void * context);
		T        	(*fg)	(const Vector<T> & x, Vector<T> & fg,	void * context);
		void     	(*cb)	(const Vector<T> & x, T err,         	void * context);

		void *context;

		Vector<T> x;      ///< The current point of interest.
		T fx;     ///< The value of the function at x.
		Vector<T> gx;     ///< The gradient of the function at x.

		Vector<T> old_x;  ///< The previous value of x, before the last line_search().
		T old_fx; ///< The value of the function at old_x.
		Vector<T> old_gx; ///< The gradient of the function at old_x.

		T er; ///< An indicator of the current error, for logging.
		int ler;   ///< The base-10 logarithm of er.
	};

	template <typename T> Minimizer<T>::Minimizer (unsigned int n_, T f_ (const Vector<T> &, void *), Vector<T> g_ (const Vector<T> &, void *),
			T fg_ (const Vector<T> &, Vector<T> &, void *), void *context_) :
	    n(n_), f(f_), g(g_), fg(fg_), cb(NULL), context(context_), x(n), gx(n), old_x(n), old_gx(n), er(1.0), ler(0) {}

	template <typename T> Minimizer<T>::Minimizer (unsigned int n_, T fg_ (const Vector<T> &, Vector<T> &, void *), void *context_) :
		Minimizer (n_,NULL,NULL,fg_,context_) {}

	template <typename T> Minimizer<T>::Minimizer (unsigned int n_, T f_ (const Vector<T> &, void *), Vector<T> g_ (const Vector<T> &, void *), void *context_) :
		Minimizer (n_,f_,g_,NULL,context_) {}

	template <typename T> T Minimizer<T>::compute (const Vector<T> &x_) {
		if ((!x_.empty()) && (&x != &x_)) x=x_;

		if (fg)	{ fx = fg(x,gx,context); }
		else   	{ fx = f(x,context); gx = g(x,context); }

		return fx;
	}

	template <typename T> void Minimizer<T>::line_search (const Vector<T> &d) {
		old_x.swap(x); old_fx=fx; old_gx.swap(gx);

		T qq_0 = T(.8) * inner_prod (old_gx,d);
		T dir = (qq_0>T(0) ? T(-1) : T(1));
		T t_l = T(0), t_r = T(0), t = dir;
		T y;

		bool refining = false;

		while (true) {
			x = old_x + t*d; compute();
			y = old_fx + T(.3) * t * qq_0;

			if ((fx<=y) && (dir*inner_prod (gx,d) >= dir*qq_0)) break;
			if (fx>y) { t_r=t; refining = true; } else t_l = t;
			if (refining) t = (t_r+t_l)/T(2); else t *= T(2);
			if (t-t_l+T(1) == T(1)) break;
		}
	}

	template <typename T> T Minimizer<T>::minimize_grad (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		while (fx < old_fx) line_search (Vector<T>(gx));
		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+1;
		old_gx = gx;

		Vector<T> dx,dg,Wdg;
		T dgdx,u;

		Vector<T> diag = W0;
		if (diag.size() == 0) diag = Vector<T> (x0.size(), T(1.0));
		Matrix W(x0.size(),x0.size());
		for (unsigned int i=0; i<x0.size(); ++i) W(i,i) = diag[i];

		while (fx < old_fx) {
			line_search(prod(W,gx));

			dx = x - old_x;
			dg = gx - old_gx;
			Wdg = prod(W,dg);
			dgdx = inner_prod(dg,dx);
			dx /= dgdx;
			u = dgdx + inner_prod(dg,Wdg);

			W += outer_prod (u*dx-Wdg,dx) - outer_prod (dx,Wdg);
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
				T c = inner_prod(gx,gx) / inner_prod(old_gx,old_gx);
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
		Vector<T> y(n);

		bool first = true;

		while (fx < old_fx) {
			old_d.swap(d);
			d = -gx;

			if (!first) {
				T c1 = inner_prod(old_gx,old_gx);

				old_gx -= gx;

				old_d *= inner_prod(old_gx,gx) / c1;
				d -= old_d;
			}

			line_search(d);
			first=false;
		}

		return fx;
	}

	template <typename T> T Minimizer<T>::minimize_qn (const Vector<T> &x0) {
		compute(x0);
		old_x  = x;
		old_fx = fx+T(1);
		old_gx = gx;

		Vector<T> d(n);
		Vector<T> old_d(n);
		Vector<T> y(n);

		bool first = true;

		while (fx < old_fx) {
			old_d.swap(d);
			d = -gx;

			if (!first) {
				y = gx - old_gx;
				d += old_d * inner_prod(y,gx) / inner_prod(y,old_d);
			}

			line_search(d);
			first=false;

			if (cb) cb(x,fx,context);
		}

		return fx;
	}
};
