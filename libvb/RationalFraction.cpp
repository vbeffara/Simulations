#include <vb/RationalFraction.h>

namespace vb {
	RationalFraction::RationalFraction (Polynomial P_, Polynomial Q_) : P(P_), Q(Q_) {}

	cpx RationalFraction::operator() (cpx z) const { return P(z)/Q(z); }

	int RationalFraction::degree () const { return P.degree() - Q.degree(); }

	void RationalFraction::add_root (cpx z) { P.add_root(z); }
	void RationalFraction::add_pole (cpx z) { Q.add_root(z); }

	RationalFraction RationalFraction::derivative () const {
		const Polynomial PP=P.derivative(), QQ=Q.derivative();
		return {Q*PP-P*QQ,Q*Q};
	}

	std::ostream & operator<< (std::ostream & os, const RationalFraction & R) {
		os << "P[" << R.P.v << "_] := " << R.P << std::endl;
		os << "Q[" << R.P.v << "_] := " << R.Q << std::endl;
		return os;
	}
}
