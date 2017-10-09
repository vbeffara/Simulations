#pragma once
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> int Polynomial<T>::degree () const { return size()-1; }

	template <typename T> Polynomial<T> Polynomial<T>::derivative () const {
		std::vector<T> out (degree());
		for (int i=0; i<degree(); ++i) out[i] = T(i+1) * at(i+1);
		if (out.empty()) out.push_back(T(0));
		return out;
	}

	template <typename T> void Polynomial<T>::add_root (T l) {
		push_back(T(0));
		for (int i=degree(); i>0; --i) at(i) = at(i-1) - l*at(i);
		at(0) = -l*at(0);
	}

	template <typename T> Polynomial<T> operator* (const T & c, const Polynomial<T> & Q) {
		std::vector<T> out (Q.degree()+1); for (int i=0; i<out.size(); ++i) out[i] = c*Q[i]; return out;
	}

	template <typename T> Polynomial<T> operator* (const Polynomial<T> & P, const Polynomial<T> & Q) {
		std::vector<T> PQ (P.degree()+Q.degree()+1, T(0));
		for (int i=0; i<=P.degree(); ++i) {
			for (int j=0; j<=Q.degree(); ++j) {
				PQ[i+j] += P[i] * Q[j];
			}
		}
		return PQ;
	}

	template <typename T> Polynomial<T> operator- (const Polynomial<T> & P, const Polynomial<T> & Q) {
		Polynomial<T> PQ = P;
		if (PQ.degree()<Q.degree()) PQ.resize(Q.degree()+1, T(0));
		for (int i=0; i<=Q.degree(); ++i) PQ[i] -= Q[i];
		return PQ;
	}

	template <typename T> template <typename U> U Polynomial<T>::operator() (U z) const {
		auto out = U(back());
		for (int i=size()-1; i>0; --i) out = out*z+U(at(i-1));
		return out;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Polynomial<T> & P) {
		double eps = pow(10.0,-os.precision());
		bool first=true;
		for (int j=P.degree()+1; j>0; --j) { int i=j-1;
			if (abs(P[i]) <= abs(T(eps))) continue;
			os << (first ? "" : " + ");
			if ((i==0) || (abs(P[i]-T(1)) > abs(T(eps)))) os << P[i];
			if (i>0) os << " " << P.v;
			if (i>1) os << "^" << i;
			first = false;
		}
		if (first) os << 0;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Polynomial<bigint> & P) {
		bool first=true;
		for (int j=P.degree()+1; j>0; --j) { int i=j-1;
			if (P[i] == 0) continue;
			os << (first ? "" : " + ");
			if ((i==0) || (P[i] != 1)) os << P[i];
			if (i>0) os << " " << P.v;
			if (i>1) os << "^" << i;
			first = false;
		}
		if (first) os << 0;
		return os;
	}

	template <> std::ostream & operator<< (std::ostream & os, const Polynomial<cpxint> & P) {
		bool first=true;
		for (int j=P.degree()+1; j>0; --j) { int i=j-1;
			if (P[i] == cpxint(0)) continue;
			os << (first ? "" : " + ");
			if ((i==0) || (P[i] != cpxint(1))) os << P[i];
			if (i>0) os << " " << P.v;
			if (i>1) os << "^" << i;
			first = false;
		}
		if (first) os << 0;
		return os;
	}
}
