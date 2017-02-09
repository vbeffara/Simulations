#include <vb/Hub.h>
#include <vb/LinearAlgebra.h>
#include <Eigen/Dense>

using namespace vb; using namespace cln; using namespace std;

namespace cln {
	// inline cl_R abs2 (const cl_R & z) { return norm(z); }
	inline cl_R abs2 (const cl_N & z) { return norm(z); }
	inline cl_N conj (const cl_N & z) { return conjugate(z); }
	inline cl_R real (const cl_N & z) { return realpart(z); }
	inline cl_R imag (const cl_N & z) { return imagpart(z); }
}

namespace Eigen {
	template<> struct NumTraits <cl_R> : GenericNumTraits<cl_R> {
		using Real = cl_R;
		using NonInteger = cl_R;
		using Literal = cl_R;
		using Nested = cl_R;

		static inline Real epsilon() { return 0; }
		static inline int dummy_precision() { return 0; }
		static inline int digits10() { return 0; }

		enum {
			IsInteger = 0,
			IsSigned = 1,
			IsComplex = 0,
		};
	};

	template<> struct NumTraits <cl_N> : GenericNumTraits <cl_N> {
		using Real = cl_R;
		using NonInteger = cl_N;
		using Literal = cl_N;
		using Nested = cl_N;

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

template <typename T> void uBlas (string s) {
	Matrix<T> m (3,3); for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	Vector<T> v (3); for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	auto x = solve(m,v);

	H.L->info ("uBLAS  | {} | Matrix   m = {}", s, m);
	H.L->info ("uBLAS  | {} | Vector   v = {}", s, v);
	H.L->info ("uBLAS  | {} | Solution x = {}", s, x);
}

template <typename T> void Eigen3 (string s) {
	Eigen::Matrix <T,3,3> m; for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	Eigen::Matrix <T,3,1> v; for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	Eigen::Matrix <T,3,1> x = m.colPivHouseholderQr().solve(v);

	H.L->info ("Eigen3 | {} | Matrix   m = {}", s, m);
	H.L->info ("Eigen3 | {} | Vector   v = {}", s, v.transpose());
	H.L->info ("Eigen3 | {} | Solution x = {}", s, x.transpose());
}

int main () {
	uBlas <double>  ("double");
	uBlas <cl_R>    ("CLR   ");
	uBlas <cl_N>    ("CLN   ");
	Eigen3 <double> ("double");
	Eigen3 <cl_R>   ("CLR   ");
	Eigen3 <cl_N>   ("CLN   ");
}
