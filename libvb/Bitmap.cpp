/// @file
/// Implementation of the vb::Bitmap class.

#include <vb/Bitmap.h>

namespace vb {
  template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, int d) :
    Picture(wd,ht,tit),
    stage ((Color *) (surface -> get_data())),
    data (0), z0(0)
  { }

  template<> Color & Bitmap<Color>::at (coo z) {
    return stage[real(z) + stride*imag(z)];
  }

  template<> Color const & Bitmap<Color>::at (coo z) const {
    return stage[real(z) + stride*imag(z)];
  }

  template<> Color & Bitmap<Color>::at (int x, int y) {
    return stage[x+stride*y];
  }

  template<> Color const & Bitmap<Color>::at (int x, int y) const {
    return stage[x+stride*y];
  }
}
