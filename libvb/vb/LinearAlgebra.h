#pragma once
#define BOOST_UBLAS_TYPE_CHECK 0
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace vb {
	namespace ublas = boost::numeric::ublas;

	template <typename T> using Vector = ublas::vector<T>;
	template <typename T> using Matrix = ublas::matrix<T>;

	template <typename T> T det (const Matrix<T> & input) {
		Matrix<T> A(input);
		ublas::permutation_matrix<> pivots(A.size1());
		if (lu_factorize(A,pivots)) return T(0);

		T det(1);
		for (unsigned i=0; i<pivots.size(); ++i) {
			if (pivots(i)!=i) det *= T(-1);
			det *= A(i,i);
		}
		return det;
	}

	template <typename T> bool inv (const Matrix<T> & input, Matrix<T> & inverse) {
		Matrix<T> A(input);
		ublas::permutation_matrix<> pivots(A.size1());
		if (lu_factorize(A,pivots)) return false;

		inverse.assign (ublas::identity_matrix<T>(A.size1()));
		lu_substitute (A,pivots,inverse);
		return true;
	}

	template <typename T> void printmath (std::ostream & os, const Vector<T> & v) {
		os << "{"; bool first=true;
		for (unsigned i=0; i<v.size(); ++i) { os << (first ? " " : ", ") << v(i); first = false; }
		os << "}";
	}

	template <typename T> void printmath (std::ostream & os, const Matrix<T> & A) {
		bool ffirst=true; for (unsigned i=0; i<A.size1(); ++i) {
			os << (ffirst ? "{{" : " },\n{"); ffirst = false;
			bool first=true; for (unsigned j=0; j<A.size2(); ++j) { os << (first ? " " : ", ") << A(i,j); first = false; }
		} os << " }}";
	}
}
