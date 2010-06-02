/// @file
/// Implementation of the vb::Bitmap class.

#include <vb/Bitmap.h>

namespace vb {
  template<> Color Bitmap<void>::color_at (int x, int y) { return stage[x+stride*y]; }
}
