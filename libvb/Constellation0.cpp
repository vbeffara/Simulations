#include <vb/impl/Constellation0.hxx>

namespace vb {
	template class Constellation0<double>;
	template class Constellation0<real_t>;

	template Constellation0<real_t>::Constellation0 (const Constellation0<double> &);

	template std::ostream & operator<< (std::ostream & os, const Constellation0<double> & C);
	template std::ostream & operator<< (std::ostream & os, const Constellation0<real_t> & C);
}
