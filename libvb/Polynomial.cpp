#include <vb/Polynomial.h>

namespace vb {
	Polynomial::Polynomial (const std::vector<cpx> V) : std::vector<cpx> (V) {}

	void Polynomial::add_root (cpx l) {
		push_back(0);
		for (int i=size()-1; i>0; --i) at(i) = at(i-1) - l*at(i);
		at(0) = -l*at(0);
	}

	cpx Polynomial::operator() (cpx z) const {
		cpx out = back();
		for (int i=size()-2; i>=0; --i) out = out*z+at(i);
		return out;
	}

	std::ostream & operator<< (std::ostream & os, const Polynomial & P) {
		bool first=true;
		for (int i=P.size()-1; i>=0; --i) {
			if (P[i]==0.0) continue;
			os << (first ? "(" : " + (");
			if (imag(P[i])==0.0) os << real(P[i]); else
			if (real(P[i])==0.0) os << imag(P[i]) << " I"; else
			os << real(P[i]) << (imag(P[i])>=0 ? "+" : "") << imag(P[i]) << "I";
			os << ")";
			if (i>0) os << " " << P.v << "^" << i;
			first = false;
		}
		return os;
	}
}
