#include <vb/Constellation_impl.h>

namespace vb {
	template class Constellation<double>;
	template class Constellation<gmp100>;

	template Constellation<gmp100>::Constellation (const Constellation<double> &);
}
