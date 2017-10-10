#include <vb/impl/LinearAlgebra.hxx>

namespace vb {
	template Vector<double>   	solve<double>   	(const Matrix<double> &,   	const Vector<double> &);
	template Vector<cpx>      	solve<cpx>      	(const Matrix<cpx> &,      	const Vector<cpx> &);
	template Vector<real_t>   	solve<real_t>   	(const Matrix<real_t> &,   	const Vector<real_t> &);
	template Vector<complex_t>	solve<complex_t>	(const Matrix<complex_t> &,	const Vector<complex_t> &);
} // namespace vb
