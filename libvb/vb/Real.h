/// @file Real.h
/// Definitions of types for real and complex numbers.
///
/// If it can find gmpxx.h it will use GMP (for arbitrary precision), in 
/// which case you can define REAL_PRECISION to the number of needed 
/// significant bits.

#ifndef __VB_REAL_H
#define __VB_REAL_H

#include <complex>
#include <vb/config.h>
#include <math.h>

#ifdef HAVE_CLN
#define WANT_OBFUSCATING_OPERATORS
#include <cln/cln.h>
#endif

namespace vb {
#ifdef HAVE_CLN
  extern int real_precision;

  class Real : public cln::cl_R {
    public:
      Real (int x = 0) : cln::cl_R (cln::cl_float(x, cln::float_format(real_precision))) { }
      Real (unsigned int x) : cln::cl_R (cln::cl_float(x, cln::float_format(real_precision))) { }
      Real (double x) : cln::cl_R (cln::cl_float(x, cln::float_format(real_precision))) { }
      Real (size_t x) : cln::cl_R (cln::cl_float(int(x), cln::float_format(real_precision))) { }
      Real (const cln::cl_R &x) : cln::cl_R (x) { }
  };

  inline double get_d (const Real &x) { return cln::double_approx(x); }

  inline Real atan2 (const Real &y, const Real &x) { return atan(x,y); }

  typedef std::complex<Real> cpx;   ///< Utility type for a complex number.
#else
  typedef double Real;              ///< Utility type for a real number, if I ever want to use GMP.

  /// Return the smaller of two real numbers.
  inline Real min (Real x, Real y) { return (x<y?x:y); }

  /// Return the larger of two real numbers.
  inline Real max (Real x, Real y) { return (x>y?x:y); }

  /** Return the value of a vb::Real as a double.
   *
   * @param x The number in question.
   */

  inline double get_d (const Real &x) { return x; }

  typedef std::complex<Real> cpx;   ///< Utility type for a complex number.

  inline double realpart (cpx z) { return z.real(); }
#endif

  /// This is equal to Pi.
#define ONE_PI ((Real)3.14159265358979)

  /// This is equal to twice Pi.
#define TWO_PI ((Real)6.28318530717958)
}

#endif
