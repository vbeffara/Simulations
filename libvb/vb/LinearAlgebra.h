#pragma once
#include <vb/cpx.h>
#undef Success
#include <Eigen/Dense>

namespace Eigen {
	template<> struct NumTraits <real_t> : GenericNumTraits<real_t> {
		using Real = real_t;
		using NonInteger = real_t;
		using Literal = real_t;
		using Nested = real_t;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 0,
		};
	};

	template<> struct NumTraits <complex_t> : GenericNumTraits <complex_t> {
		using Real = real_t;
		using NonInteger = complex_t;
		using Literal = complex_t;
		using Nested = complex_t;

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
	template <typename T> using Vector = Eigen::Matrix <T,Eigen::Dynamic,1>;
	template <typename T> using Matrix = Eigen::Matrix <T,Eigen::Dynamic,Eigen::Dynamic>;

	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y);
}
