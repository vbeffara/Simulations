#include <vb/impl/Constellation1.hxx>

namespace vb {
	template class Constellation1<double>;
	template class Constellation1<real_t>;

	template Constellation1<real_t>::Constellation1 (const Constellation1<double> &);

	template std::ostream & operator<< (std::ostream & os, const Constellation1<double> & C);
}
