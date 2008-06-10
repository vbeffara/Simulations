/// @file types.h
/// Definitions of a few convenience types.

#ifndef __VB_TYPES_H
#define __VB_TYPES_H

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

#ifdef HAVE_GMPXX
  class Real {
    public:
      mpf_class _r;

      Real ()              : _r(mpf_class(0.0,REAL_PRECISION)) { };
      Real (const Real &r) : _r(r._r)                          { };
      Real (mpf_class r)   : _r(r)                             { };
      Real (double r)      : _r(mpf_class(r,REAL_PRECISION))   { };

      // operator mpf_class () const { return _r; }

      Real operator- () const { return Real (-_r); }

      Real operator+= (Real other) { _r += other._r; return (*this); }
      Real operator-= (Real other) { _r -= other._r; return (*this); }
      Real operator*= (Real other) { _r *= other._r; return (*this); }
      Real operator/= (Real other) { _r /= other._r; return (*this); }

      double get_d() const { return _r.get_d(); }
  };

  inline bool operator== (const Real &x, const Real &y) { return x._r == y._r; }
  inline bool operator!= (const Real &x, const Real &y) { return x._r != y._r; }

  inline bool operator< (const Real &x, const Real &y) { return x._r < y._r; }
  inline bool operator> (const Real &x, const Real &y) { return x._r > y._r; }
  inline bool operator<= (const Real &x, const Real &y) { return x._r <= y._r; }
  inline bool operator>= (const Real &x, const Real &y) { return x._r >= y._r; }

  inline Real operator+ (const Real &x, const Real &y) { return Real (x._r+y._r); }
  inline Real operator- (const Real &x, const Real &y) { return Real (x._r-y._r); }
  inline Real operator* (const Real &x, const Real &y) { return Real (x._r*y._r); }
  inline Real operator/ (const Real &x, const Real &y) { return Real (x._r/y._r); }

  inline std::ostream &operator<< (std::ostream &o, const Real &x) { return o << x._r; }

  inline Real abs (const Real &r) { return std::abs(r.get_d()); }
  inline Real fabs (const Real &r) { return std::fabs(r.get_d()); }
  inline Real exp (const Real &r) { return std::exp(r.get_d()); }
  inline Real cos (const Real &r) { return std::cos(r.get_d()); }
  inline Real acos (const Real &r) { return std::acos(r.get_d()); }
  inline Real sin (const Real &r) { return std::sin(r.get_d()); }
  inline Real sqrt (const Real &r) { return std::sqrt(r.get_d()); }

  inline Real atan2 (const Real &x, const Real &y) { return Real (atan2(x._r,y._r)); }
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
