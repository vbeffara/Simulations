/// @file
/// Implementation of the vb::Bitmap class.

#include <vb/Bitmap.h>

namespace vb {
  template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, Color d) :
    Picture(wd,ht,tit),
    stage ((Color *) (cairo_image_surface_get_data (surface))),
    data(stage), z0(0), dflt(d)
  { }
}
