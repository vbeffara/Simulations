#include <vb/Polynomial_impl.h>

namespace vb {
	template class Polynomial<bigint>;
	template class Polynomial<double>;
	template class Polynomial<gmp100>;

	template class Polynomial<cpxint>;
	template class Polynomial<cpx>;
	template class Polynomial<cpx100>;

	template cpx Polynomial<cpx>::operator() (cpx z) const;
	template cpx100 Polynomial<cpxint>::operator() (cpx100 z) const;
	template cpx100 Polynomial<cpx100>::operator() (cpx100 z) const;
	template gmp100 Polynomial<bigint>::operator() (gmp100 z) const;

	template Polynomial<bigint> operator* (const bigint & c, const Polynomial<bigint> & Q);
	template Polynomial<cpxint> operator* (const cpxint & c, const Polynomial<cpxint> & Q);

	template std::ostream & operator<< (std::ostream & os, const Polynomial<cpx> & P);
	template std::ostream & operator<< (std::ostream & os, const Polynomial<cpx100> & P);
}
