#include <vb/impl/Polynomial.hxx>

namespace vb {
	template class Polynomial<bigint>;
	template class Polynomial<double>;
	template class Polynomial<real_t>;

	// template class Polynomial<cpxint>;
	template class Polynomial<cpx>;
	template class Polynomial<complex_t>;

	template cpx Polynomial<cpx>::operator() (cpx z) const;
	// template complex_t Polynomial<cpxint>::operator() (complex_t z) const;
	template complex_t Polynomial<complex_t>::operator() (complex_t z) const;
	template real_t Polynomial<bigint>::operator() (real_t z) const;

	template Polynomial<bigint> operator* (const bigint & c, const Polynomial<bigint> & Q);
	template Polynomial<cpxint> operator* (const cpxint & c, const Polynomial<cpxint> & Q);

	template std::ostream & operator<< (std::ostream & os, const Polynomial<cpx> & P);
	template std::ostream & operator<< (std::ostream & os, const Polynomial<complex_t> & P);
}
