/// @file
/// Implementation of the vb::Bitmap class.

#include <vb/Bitmap.h>

namespace vb {
  Bitmap::Bitmap (int wd, int ht, const std::string &tit)
    : AutoWindow(wd,ht,tit), stage ((Color *) (surface -> get_data()))
  { }

  void Bitmap::paint () {
    for (int x=0; x<width; ++x)
      for (int y=0; y<height; ++y)
        stage[x+stride*y] = color_at(x,y);
  }
}
