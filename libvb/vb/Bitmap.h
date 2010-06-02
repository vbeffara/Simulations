/// @file
/// Definitions for the vb::Bitmap class

#ifndef __VB_BITMAP_H
#define __VB_BITMAP_H

#include <vb/AutoWindow.h>

namespace vb {
  /** A templated base class for all bitmap pictures.
   *
   * It provides the general machinery for drawing a picture onto the 
   * screen, kind of an adaptor between vb::AutoWindow and things like 
   * vb::Image, vb::CoarseImage etc.
   */

  template <typename T> class Bitmap : public AutoWindow {
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

      virtual Color color_at (int x, int y) { return data[x+stride*y]; }

      Color * stage; ///< The raw pixel data of the screen representation.
      T * data;      ///< The actual data.

    private:
      virtual void paint ();
  };

  template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit)
    : AutoWindow(wd,ht,tit), stage ((Color *) (surface -> get_data())), data(NULL)
  { }

  template<typename T> void Bitmap<T>::paint () {
    for (int x=0; x<width; ++x)
      for (int y=0; y<height; ++y)
        stage[x+stride*y] = color_at(x,y);
  }

  template<> Color Bitmap<void>::color_at (int x, int y);
}

#endif
