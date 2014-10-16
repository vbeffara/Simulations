#include <vb/Polynomial.h>

namespace vb {
	Polynomial::Polynomial (const std::vector<cpx> V) : std::vector<cpx> (V) {}

	void Polynomial::add_root (cpx l) {
		push_back(0);
		for (int i=size()-1; i>0; --i) at(i) = at(i-1) - l*at(i);
		at(0) = -l*at(0);
	}

	unsigned Polynomial::degree () const { return size()-1; }

	Polynomial Polynomial::derivative () const {
		std::vector<cpx> out (degree());
		for (int i=0; i<degree(); ++i) out[i] = cpx(i+1) * at(i+1);
		return out;
	}

	cpx Polynomial::operator() (cpx z) const {
		cpx out = back();
		for (int i=size()-2; i>=0; --i) out = out*z+at(i);
		return out;
	}

	std::ostream & operator<< (std::ostream & os, const Polynomial & P) {
		bool first=true;
		for (int i=P.size()-1; i>=0; --i) {
			if (abs(P[i])    	<= P.precision) continue;
			if (abs(P[i]-1.0)	<= P.precision) { if (i==0) os << 1; else os << P.v << "^" << i; first=false; continue; }
			if (abs(P[i]+1.0)	<= P.precision) { if (i==0) os << -1; else os << "- " << P.v << "^" << i; first=false; continue; }
			os << (first ? "(" : " + (");
			if (fabs(imag(P[i])) <= P.precision) os << real(P[i]); else
			if (fabs(real(P[i])) <= P.precision) os << imag(P[i]) << " I"; else
			os << real(P[i]) << (imag(P[i])>=0 ? "+" : "") << imag(P[i]) << "I";
			os << ")";
			if (i>0) os << " " << P.v << "^" << i;
			first = false;
		}
		if (first) os << 0;
		return os;
	}
}
