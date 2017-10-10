#include <gsl/gsl_sf_bessel.h>
#include <vb/math.h>

namespace vb {
	double BesselJ (int n, double z) {
		return gsl_sf_bessel_Jn (n,z);
	}
} // namespace vb
