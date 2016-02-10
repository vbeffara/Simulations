#include <vb/impl/Constellation1.hxx>

namespace vb {
	template class Constellation1<double>;
	template class Constellation1<gmp100>;

	template Constellation1<gmp100>::Constellation1 (const Constellation1<double> &);

	template std::ostream & operator<< (std::ostream & os, const Constellation1<double> & C);
	template std::ostream & operator<< (std::ostream & os, const Constellation1<gmp100> & C);
}
