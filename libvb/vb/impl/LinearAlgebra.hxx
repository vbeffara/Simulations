#pragma once

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

namespace Eigen {
	template<> struct NumTraits <cln::cl_R> : GenericNumTraits<cln::cl_R> {
		using Real = cln::cl_R;
		using NonInteger = cln::cl_R;
		using Literal = cln::cl_R;
		using Nested = cln::cl_R;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 0,
		};
	};

	template<> struct NumTraits <cln::cl_N> : GenericNumTraits <cln::cl_N> {
		using Real = cln::cl_R;
		using NonInteger = cln::cl_N;
		using Literal = cln::cl_N;
		using Nested = cln::cl_N;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 1,
		};
	};
}

namespace vb {
	namespace ublas = boost::numeric::ublas;

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
		return A.colPivHouseholderQr().solve(y);
	}
}
