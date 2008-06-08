/// @file types.h
/// Definitions of a few convenience types.

#ifndef __VB_TYPES_H
#define __VB_TYPES_H

#include <complex>
#include <vb/config.h>

namespace vb {
  typedef double real;              ///< Utility type for a real number, if I ever want to use GMP.
  typedef std::complex<real> cpx;   ///< Utility type for a complex number.

  /// Return the smaller of two real numbers.
  inline real min (real x, real y) { return (x<y?x:y); }

  /// Return the larger of two real numbers.
  inline real max (real x, real y) { return (x>y?x:y); }

/// This is equal to Pi.
#define ONE_PI ((real)3.14159265358979)

/// This is equal to twice Pi.
#define TWO_PI ((real)6.28318530717958)
}

#endif
