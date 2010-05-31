/// @file
/// Definitions for the vb::Bitmap class

#ifndef __VB_BITMAP_H
#define __VB_BITMAP_H

#include <vb/AutoWindow.h>

namespace vb {
  /** An abstract base class for all bitmap pictures.
   *
   * It provides the general machinery for drawing a picture onto the 
   * screen, kind of an adaptor between vb::AutoWindow and things like 
   * vb::Image, vb::CoarseImage etc.
   */

  class Bitmap : public AutoWindow {
    public:
      /** The standard constructor of the Image class.
       *
       * @param wd The width in pixels.
       * @param ht The height in pixels.
       * @param tit The title of the image.
       */

      Bitmap (int wd, int ht, const std::string &tit);

      /** Return the vb::Color of the image at point (x,y).
       *
       * The goal of a subclass is just to provide an implementation of 
       * this, through any means necessary.
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      virtual Color color_at (int x, int y) =0;

      Color * stage; ///< The raw pixel data of the screen representation.

    private:
      virtual void paint ();
  };
}

#endif
