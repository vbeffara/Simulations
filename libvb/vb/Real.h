/// @file Real.h
/// Definitions of types for real and complex numbers.
//
// If it can fing gmpxx.h it will use GMP (for arbitrary precision), in 
// which case you can define REAL_PRECISION to the number of needed 
// significant bits.

#ifndef __VB_REAL_H
#define __VB_REAL_H

#include <complex>
#include <iostream>
#include <vb/config.h>
#include <math.h>

#ifdef HAVE_GMPXX
#include <gmpxx.h>

#ifndef REAL_PRECISION
#define REAL_PRECISION 64
#endif
#endif

namespace vb {

#ifdef HAVE_GMPXX_THIS_IS_DISABLED_FOR_NOW
  class Real {
    public:
      mpf_class _r;

      Real ();
      Real (const Real &r);
      Real (mpf_class r);
      Real (double r);

      Real operator- () const;
      Real operator+= (const Real &other);
      Real operator-= (const Real &other);
      Real operator*= (const Real &other);
      Real operator/= (const Real &other);

      double get_d() const;
  };

  bool operator== (const Real &x, const Real &y);
  bool operator!= (const Real &x, const Real &y);

  bool operator< (const Real &x, const Real &y);
  bool operator> (const Real &x, const Real &y);
  bool operator<= (const Real &x, const Real &y);
  bool operator>= (const Real &x, const Real &y);

  Real operator+ (const Real &x, const Real &y);
  Real operator- (const Real &x, const Real &y);
  Real operator* (const Real &x, const Real &y);
  Real operator/ (const Real &x, const Real &y);

  std::ostream &operator<< (std::ostream &o, const Real &x);

  Real abs (const Real &r);
  Real fabs (const Real &r);
  Real exp (const Real &r);
  Real cos (const Real &r);
  Real acos (const Real &r);
  Real sin (const Real &r);
  Real sqrt (const Real &r);

  Real atan2 (const Real &x, const Real &y);

  inline double get_d (const Real &x) { return x.get_d(); }
#else
  typedef double Real;              ///< Utility type for a real number, if I ever want to use GMP.

  inline double get_d (double x) { return x; }
#endif

  typedef std::complex<Real> cpx;   ///< Utility type for a complex number.

  /// Return the smaller of two real numbers.
  inline Real min (Real x, Real y) { return (x<y?x:y); }

  /// Return the larger of two real numbers.
  inline Real max (Real x, Real y) { return (x>y?x:y); }

  /// This is equal to Pi.
#define ONE_PI ((Real)3.14159265358979)

  /// This is equal to twice Pi.
#define TWO_PI ((Real)6.28318530717958)
}

#endif
