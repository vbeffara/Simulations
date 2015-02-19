#include <vb/Constellation0_impl.h>

namespace vb {
	template class Constellation0<double>;
	template class Constellation0<gmp100>;

	template Constellation0<gmp100>::Constellation0 (const Constellation0<double> &);

	template std::ostream & operator<< (std::ostream & os, const Constellation0<double> & C);
	template std::ostream & operator<< (std::ostream & os, const Constellation0<gmp100> & C);
}
