/// @file types.h
/// Definitions of a few convenience types.

#ifndef __VB_TYPES_H
#define __VB_TYPES_H

#include <complex>
#include <vb/config.h>

#ifdef HAVE_GMPXX
#include <gmpxx.h>

#ifndef REAL_PRECISION
#define REAL_PRECISION 64
#endif
#endif

namespace vb {

#ifdef HAVE_GMPXX
  class Real {
    public:
      mpf_class _r;

      Real ()              : _r(mpf_class(0.0,REAL_PRECISION)) { };
      Real (const Real &r) : _r(r._r)                          { };
      Real (mpf_class r)   : _r(r)                             { };
      Real (double r)      : _r(mpf_class(r,REAL_PRECISION))   { };

      operator double () const { return _r.get_d(); }

      Real operator- () const { return Real (-_r); }

      bool operator== (const Real &other) const { return _r == other._r; }
      bool operator!= (const Real &other) const { return _r != other._r; }

      bool operator< (Real &other) { return _r < other._r; }
      bool operator> (Real &other) { return _r > other._r; }

      Real operator+= (Real other) { _r += other._r; return (*this); }
      Real operator-= (Real other) { _r -= other._r; return (*this); }
      Real operator*= (Real other) { _r *= other._r; return (*this); }
      Real operator/= (Real other) { _r /= other._r; return (*this); }

      Real operator+ (const Real &other) { return Real (_r + other._r); }
      Real operator+ (double other)      { return Real (_r + other); }

      Real operator- (const Real &other) { return Real (_r - other._r); }
      Real operator* (const Real &other) { return Real (_r * other._r); }

      Real operator/ (const Real &other) { return Real (_r / other._r); }
      Real operator/ (double other)      { return Real (_r / other); }
      Real operator/ (long other)        { return Real (_r / other); }
      Real operator/ (int other)         { return Real (_r / other); }

      double get_d() { return _r.get_d(); }
  };

  //inline Real exp (const Real &r) { return Real (exp(r)); }
  inline Real sqrt (const Real &r) { return Real ((mpf_class) sqrt(r._r)); }
#else
  typedef double Real;              ///< Utility type for a real number, if I ever want to use GMP.
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
