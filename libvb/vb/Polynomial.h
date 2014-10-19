#pragma once
#include <vb/cpx.h> // for operator<< override
#include <iostream>
#include <vector>

namespace vb {
	template <typename T> class Polynomial : public std::vector<T> {
	public:
		Polynomial (const std::vector<T> V) : std::vector<T> (V) {};

		using std::vector<T>::at; using std::vector<T>::size; using std::vector<T>::back; using std::vector<T>::push_back;

		T operator() (T z) const;

		unsigned     	degree    	() const;
		Polynomial<T>	derivative	() const;
		void         	add_root  	(T l);

		std::string v = "z";
	};

	template <typename T> Polynomial<T> operator- (const Polynomial<T> & P, const Polynomial<T> & Q);
	template <typename T> Polynomial<T> operator* (const Polynomial<T> & P, const Polynomial<T> & Q);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Polynomial<T> & P);

	/**********************************************************************************************************/

	template <typename T> unsigned Polynomial<T>::degree () const { return size()-1; }

	template <typename T> Polynomial<T> Polynomial<T>::derivative () const {
		std::vector<T> out (degree());
		for (int i=0; i<degree(); ++i) out[i] = T(i+1) * at(i+1);
		if (out.empty()) out.push_back(0.0);
		return out;
	}

	template <typename T> void Polynomial<T>::add_root (T l) {
		push_back(T(0));
		for (int i=degree(); i>0; --i) at(i) = at(i-1) - l*at(i);
		at(0) = -l*at(0);
	}

	template <typename T> Polynomial<T> operator* (const Polynomial<T> & P, const Polynomial<T> & Q) {
		std::vector<T> PQ (P.degree()+Q.degree()+1, 0.0);
		for (int i=0; i<=P.degree(); ++i) {
			for (int j=0; j<=Q.degree(); ++j) {
				PQ[i+j] += P[i] * Q[j];
			}
		}
		return PQ;
	}

	template <typename T> Polynomial<T> operator- (const Polynomial<T> & P, const Polynomial<T> & Q) {
		Polynomial<T> PQ = P;
		if (PQ.degree()<Q.degree()) PQ.resize(Q.degree()+1,0.0);
		for (int i=0; i<=Q.degree(); ++i) PQ[i] -= Q[i];
		return PQ;
	}

	template <typename T> T Polynomial<T>::operator() (T z) const {
		T out = back();
		for (int i=size()-2; i>=0; --i) out = out*z+at(i);
		return out;
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Polynomial<T> & P) {
		double eps = pow(10.0,-os.precision());
		bool first=true;
		for (int i=P.degree(); i>=0; --i) {
			if (abs(P[i])     	<= eps) continue;
			if (abs(P[i]-T(1))	<= eps) { if (i==0) os << 1; 	else { os <<        	P.v; if (i>1) os << "^" << i; } first=false; continue; }
			if (abs(P[i]+T(1))	<= eps) { if (i==0) os << -1;	else { os << "- " <<	P.v; if (i>1) os << "^" << i; } first=false; continue; }
			os << (first ? "" : " + ");
			os << P[i];
			if (i>0) os << " " << P.v; if (i>1) os << "^" << i;
			first = false;
		}
		if (first) os << 0;
		return os;
	}
}
