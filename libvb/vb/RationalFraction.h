#pragma once
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> class RationalFraction {
	public:
		RationalFraction (Polynomial<T> P_ = {{1}}, Polynomial<T> Q_ = {{1}}) : P(P_), Q(Q_) {};

		T operator() (T z) const;

		int                	degree    	() const;
		RationalFraction<T>	derivative	() const;

		void add_root (T z);
		void add_pole (T z);

		Polynomial<T> P,Q;
	};

	template <typename T> std::ostream & operator<< (std::ostream & os, const RationalFraction<T> & R);

	/*********************************************************************************/

	template <typename T> T RationalFraction<T>::operator() (T z) const { return P(z)/Q(z); }

	template <typename T> int RationalFraction<T>::degree () const { return P.degree() - Q.degree(); }

	template <typename T> void RationalFraction<T>::add_root (T z) { P.add_root(z); }
	template <typename T> void RationalFraction<T>::add_pole (T z) { Q.add_root(z); }

	template <typename T> RationalFraction<T> RationalFraction<T>::derivative () const {
		const Polynomial<T> PP=P.derivative(), QQ=Q.derivative();
		return {Q*PP-P*QQ,Q*Q};
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const RationalFraction<T> & R) {
		os << "P[" << R.P.v << "_] := " << R.P << std::endl;
		os << "Q[" << R.P.v << "_] := " << R.Q << std::endl;
		return os;
	}
}
