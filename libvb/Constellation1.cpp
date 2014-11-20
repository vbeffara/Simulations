#include <vb/Constellation1_impl.h>

namespace vb {
	template class Constellation1<double>;
	template class Constellation1<gmp100>;

	template Constellation1<gmp100> cconvert (Constellation1<double> & C);

	template std::ostream & operator<< (std::ostream & os, const Constellation1<double> & C);
	template std::ostream & operator<< (std::ostream & os, const Constellation1<gmp100> & C);
}
