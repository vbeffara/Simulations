#pragma once /// @file
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> class Minimizer { public:
		Minimizer (int n_);
		Minimizer (int n_, std::function < T (const Vector<T>&,Vector<T>*) > fg_);
		Minimizer (int n_, std::function < T (const Vector<T>&) > f_, std::function < Vector<T> (const Vector<T> &) > g_);

		T compute (const Vector<T> & x = Vector<T>(0));

		void line_search (const Vector<T> &d);

		T minimize_grad (const Vector<T> &x0);
		T minimize_bfgs (const Vector<T> &x0, const Vector<T> &W0 = Vector<T>(0));
		T minimize_fr (const Vector<T> &x0);
		T minimize_pr (const Vector<T> &x0);
		T minimize_qn (const Vector<T> &x0);

		int n=0;

		std::function < T        	(const Vector<T>&)           	> f;
		std::function < Vector<T>	(const Vector<T>&)           	> g;
		std::function < T        	(const Vector<T>&,Vector<T>*)	> fg;
		std::function < void     	(const Vector<T>&,T)         	> cb;

		Vector<T> x;    	T fx;    	Vector<T> gx;
		Vector<T> old_x;	T old_fx;	Vector<T> old_gx;

		T er;
		int ler=0;
	};
}
