/// @file
/// Definitions for the vb::Bitmap class

#ifndef __VB_BITMAP_H
#define __VB_BITMAP_H

#include <vb/Picture.h>

namespace vb {
  /** A templated base class for all bitmap pictures.
   *
   * It provides the general machinery for drawing a picture onto the
   * screen, kind of an adaptor between vb::AutoWindow and things like
   * vb::Image, vb::CoarseImage etc.
   *
   * vb::Bitmap<Color> is handled in a special way, data is not
   * initialized and the image data is handled directly.
   */

  template <typename T> class Bitmap : public Picture {
    public:
      /** The standard constructor of the Image class.
       *
       * @param  wd The width in pixels.
       * @param  ht The height in pixels.
       * @param tit The title of the image.
       * @param   d The default value at a pixel.
       */

      Bitmap (int wd, int ht, const std::string &tit, int d=0);

      /** Return an alias to the element at coordinates (x,y).
       */

      T & at (int x, int y) { return data[x+stride*y]; }

      /** Return the vb::Color of the image at point (x,y).
       *
       * The goal of a subclass is just to provide an implementation
       * of this, through any means necessary. NB this has to be
       * cheap, it is just for display. for more computationally
       * intensive coloring, use compute().
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      virtual Color color_at (int x, int y) { return data[x+stride*y]; }

      Color * stage;       ///< The raw pixel data of the screen representation.
      std::vector<T> data; ///< The actual data.

    private:
      virtual void paint ();
  };

  template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit, int d) :
    Picture(wd,ht,tit),
    stage ((Color *) (surface -> get_data())),
    data (stride*ht,d)
  { }

  template<typename T> void Bitmap<T>::paint () {
    for (int x=0; x<w(); ++x)
      for (int y=0; y<h(); ++y)
        stage[x+stride*y] = color_at(x,y);
  }

  /** The special-case constructor for Bitmap<Color>.
   *
   * For Bitmap<Color>, data is left empty and the underlying Cairo
   * surface is accessed directly through stage.
   *
   * @param  wd The width in pixels.
   * @param  ht The height in pixels.
   * @param tit The title of the image.
   * @param   d The default color.
   */

  template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, int d);

  /** Return an alias to the element at coordinates (x,y).
   *
   * For Bitmap<Color>, it points directly to the underlying Cairo
   * surface and allows for direct manipulation.
   *
   * @param x The first coordinate of the point.
   * @param y The second coordinate of the point.
   */

  template<> Color & Bitmap<Color>::at (int x, int y);

  /** Return the vb::Color of the image at point (x,y).
   *
   * This is a special case for Bitmap<Color>, it accesses the
   * underlying Cairo surface directly, ignoring data.
   *
   * @param x The first coordinate of the point.
   * @param y The second coordinate of the point.
   */

  template<> Color Bitmap<Color>::color_at (int x, int y);
}

#endif
