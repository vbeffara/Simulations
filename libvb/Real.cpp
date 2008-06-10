/// @file Real.cpp
/// Implementation of the vb::Real and vb::Complex classes.

#include <vb/Real.h>

namespace vb {
    Real::Real ()              : _r(mpf_class(0.0,REAL_PRECISION)) { };
    Real::Real (const Real &r) : _r(r._r)                          { };
    Real::Real (mpf_class r)   : _r(r)                             { };
    Real::Real (double r)      : _r(mpf_class(r,REAL_PRECISION))   { };

    Real Real::operator-  () const            { return Real (-_r); }
    Real Real::operator+= (const Real &other) { _r += other._r; return (*this); }
    Real Real::operator-= (const Real &other) { _r -= other._r; return (*this); }
    Real Real::operator*= (const Real &other) { _r *= other._r; return (*this); }
    Real Real::operator/= (const Real &other) { _r /= other._r; return (*this); }

    double Real::get_d() const { return _r.get_d(); }

    bool operator== (const Real &x, const Real &y) { return x._r == y._r; }
    bool operator!= (const Real &x, const Real &y) { return x._r != y._r; }

    bool operator< (const Real &x, const Real &y) { return x._r < y._r; }
    bool operator> (const Real &x, const Real &y) { return x._r > y._r; }
    bool operator<= (const Real &x, const Real &y) { return x._r <= y._r; }
    bool operator>= (const Real &x, const Real &y) { return x._r >= y._r; }

    Real operator+ (const Real &x, const Real &y) { return Real (x._r+y._r); }
    Real operator- (const Real &x, const Real &y) { return Real (x._r-y._r); }
    Real operator* (const Real &x, const Real &y) { return Real (x._r*y._r); }
    Real operator/ (const Real &x, const Real &y) { return Real (x._r/y._r); }

    std::ostream &operator<< (std::ostream &o, const Real &x) { return o << x._r; }

    Real abs (const Real &r) { return std::abs(r.get_d()); }
    Real fabs (const Real &r) { return std::fabs(r.get_d()); }
    Real exp (const Real &r) { return std::exp(r.get_d()); }
    Real cos (const Real &r) { return std::cos(r.get_d()); }
    Real acos (const Real &r) { return std::acos(r.get_d()); }
    Real sin (const Real &r) { return std::sin(r.get_d()); }
    Real sqrt (const Real &r) { return std::sqrt(r.get_d()); }

    Real atan2 (const Real &x, const Real &y) { return Real (atan2(x._r,y._r)); }
}
