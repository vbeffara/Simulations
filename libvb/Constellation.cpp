#include <vb/Constellation_impl.h>

namespace vb {
	template class Constellation<double>;
	template class Constellation<gmp100>;

	// template Constellation0<gmp100> cconvert (Constellation0<double> & C);

	// template std::ostream & operator<< (std::ostream & os, const Constellation0<double> & C);
	// template std::ostream & operator<< (std::ostream & os, const Constellation0<gmp100> & C);
}
