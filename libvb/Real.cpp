/// @file Real.cpp
/// Implementation of the vb::Real and vb::Complex classes.

#include <vb/Real.h>

namespace vb {
#ifdef HAVE_CLN
  int real_precision = 10;
#endif
}
