#include <vb/NumberTheory_impl.h>

namespace vb {
	template Polynomial<bigint> guess (gmp100 x, gmp100 eps);
	template Polynomial<bigint> guess (gmp100 x, int leps, unsigned d);
	template Polynomial<cpxint> guess (std::complex<gmp100> x, gmp100 eps);
	template Polynomial<cpxint> guess (std::complex<gmp100> x, int leps, unsigned d);
}
