#pragma once
#include <vb/math.h>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace boost { namespace numeric { namespace ublas {
	template <> struct type_traits <vb::complex_t> {
		// using self_type = type_traits<vb::complex_t>;
        // using value_type = vb::complex_t;
        // using precision_type = value_type;
        using real_type = vb::real_t;
        using reference = vb::complex_t &;
        using const_reference = const vb::complex_t &;

        static real_type norm_inf (const_reference t) {
			return std::max (type_traits<real_type>::type_abs (realpart(t)),
				type_traits<real_type>::type_abs (imagpart(t)));
        }
    };
}}}

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

	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y) {
		Matrix<T> AA(A); Vector<T> x(y);
		ublas::permutation_matrix<size_t> pm(A.size1());
		lu_factorize(AA, pm);
		lu_substitute(AA, pm, x);
		return x;
	}
}
