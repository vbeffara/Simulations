#include <vb/Constellation_impl.h>

namespace vb {
	template class Constellation<double>;
	template class Constellation<gmp100>;

	template Constellation<gmp100> cconvert (Constellation<double> & C);

	template std::ostream & operator<< (std::ostream & os, const Constellation<double> & C);
	template std::ostream & operator<< (std::ostream & os, const Constellation<gmp100> & C);
}
