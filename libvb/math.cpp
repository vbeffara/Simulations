#include <vb/math.h>
#include <gsl/gsl_sf_bessel.h>

namespace vb {
	double BesselJ (int n, double z) {
		return gsl_sf_bessel_Jn (n,z);
	}
}
