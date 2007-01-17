
/*
 * This  is a  dummy driver  for when  everything else  failed.  It does
 * almost nothing except defining the appropriate functions ...
 */

#include <vb/Image.h>

namespace vb {
  
  int Image::onscreen () {
    std::cerr << "Error: no output driver selected! Continuing without.\n";
    return 0;
  }
  
  void Image::update() {}
  
}
