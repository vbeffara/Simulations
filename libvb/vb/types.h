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
  class real {
    public:
      mpf_class _r;

      real::real ()              : _r(mpf_class(0.0,REAL_PRECISION)) { };
      real::real (const real &r) : _r(r._r)                          { };
      real::real (mpf_class r)   : _r(r)                             { };
      real::real (double r)      : _r(mpf_class(r,REAL_PRECISION))   { };

      operator double () const { return _r.get_d(); }

      real operator- () const { return real (-_r); }

      bool operator== (const real &other) const { return _r == other._r; }
      bool operator!= (const real &other) const { return _r != other._r; }

      bool operator< (real &other) { return _r < other._r; }
      bool operator> (real &other) { return _r > other._r; }

      real operator+= (real other) { _r += other._r; return (*this); }
      real operator-= (real other) { _r -= other._r; return (*this); }
      real operator*= (real other) { _r *= other._r; return (*this); }
      real operator/= (real other) { _r /= other._r; return (*this); }

      real operator+ (const real &other) { return real (_r + other._r); }
      real operator+ (double other)      { return real (_r + other); }

      real operator- (const real &other) { return real (_r - other._r); }
      real operator* (const real &other) { return real (_r * other._r); }

      real operator/ (const real &other) { return real (_r / other._r); }
      real operator/ (double other)      { return real (_r / other); }
      real operator/ (long other)        { return real (_r / other); }
      real operator/ (int other)         { return real (_r / other); }

      double get_d() { return _r.get_d(); }
  };

  //inline real exp (const real &r) { return real (exp(r)); }
  inline real sqrt (const real &r) { return real ((mpf_class) sqrt(r._r)); }
#else
  typedef double real;              ///< Utility type for a real number, if I ever want to use GMP.
#endif

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
