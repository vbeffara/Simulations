/// @file
/// Implementation of the vb::Bitmap class.

#include <vb/Bitmap.h>

namespace vb {
  template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, int d) :
    Picture(wd,ht,tit),
    stage ((Color *) (surface -> get_data())),
    data(stage), z0(0)
  { }
}
