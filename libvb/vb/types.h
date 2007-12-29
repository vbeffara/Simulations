/// @file types.h
/// Definitions of a few convenience types.

#ifndef __VB_TYPES_H
#define __VB_TYPES_H

#include <complex>

namespace vb {
  typedef double real;              ///< Utility type for a real number, if I ever want to use GMP.
  typedef std::complex<real> cpx;   ///< Utility type for a complex number.

  /// Return the smaller of two real numbers.
  inline real min (real x, real y) { return (x<y?x:y); }

  /// Return the larger of two real numbers.
  inline real max (real x, real y) { return (x>y?x:y); }
}

#endif
