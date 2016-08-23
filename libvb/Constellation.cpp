#include <vb/impl/Constellation.hxx>

namespace vb {
	template class Constellation<double>;
	template class Constellation<real_t>;

	template Constellation<real_t>::Constellation (const Constellation<double> &);
}
