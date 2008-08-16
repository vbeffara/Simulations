/// @file Real.cpp
/// Implementation of the vb::Real and vb::Complex classes.

#include <vb/Real.h>

namespace vb {
#ifdef HAVE_CLN
  int real_precision = 50;
  Real atan2 (const Real &y, const Real &x) { return atan(x,y); }
#endif
}
