#pragma once
#include <vb/cpx.h>
#undef Success
#include <Eigen/Dense>

namespace Eigen {
	template<> struct NumTraits <vb::real_t> : GenericNumTraits<vb::real_t> {
		using Real = vb::real_t;
		using NonInteger = vb::real_t;
		using Literal = vb::real_t;
		using Nested = vb::real_t;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 0,
		};
	};

	template<> struct NumTraits <vb::complex_t> : GenericNumTraits <vb::complex_t> {
		using Real = vb::real_t;
		using NonInteger = vb::complex_t;
		using Literal = vb::complex_t;
		using Nested = vb::complex_t;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 1,
		};
	};
} // namespace Eigen

namespace vb {
	template <typename T> using Vector = Eigen::Matrix <T,Eigen::Dynamic,1>;
	template <typename T> using Matrix = Eigen::Matrix <T,Eigen::Dynamic,Eigen::Dynamic>;

	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y);

	#ifdef UNIT_TESTS
	TEST_CASE ("Linear Algebra wrapper") {
		Matrix<cln::cl_N> m (3,3); for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
		Vector<cln::cl_N> v (3); for (int i=0; i<3; ++i) v(i) = int(3*i-2);
		Vector<cln::cl_N> x (3); x << -3.5,1.5,0;
		CHECK ((solve(m,v)-x).squaredNorm() < 1e-50);
	}
	#endif
} // namespace vb
