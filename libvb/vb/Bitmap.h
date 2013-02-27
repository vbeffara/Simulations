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

      T & at (coo z) { return data[real(z) + stride*imag(z)]; }

      T const & at (coo z) const { return data[real(z) + stride*imag(z)]; }

      void put (coo z, T const & c) { at(z) = c; step(); }

      // Deprecated (x,y) versions of the above, for backward compatibility

      T & at (int x, int y) __attribute__ ((deprecated)) { return at(coo(x,y)); }

      T const & at (int x, int y) const __attribute__ ((deprecated)) { return at(coo(x,y)); }

    private:
      Color * stage;       ///< The raw pixel data of the screen representation.
      T * data;            ///< The actual data.
      coo z0;              ///< The coordinates of the origin (at(0) is there on screen).

      virtual void paint ();
  };

  template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit, int d) :
    Picture(wd,ht,tit),
    stage ((Color *) (surface -> get_data())),
    data((T*) calloc (stride*ht,sizeof(T)))
  {
    for (int i=0; i<stride*ht; ++i) data[i]=d;
  }

  template<typename T> void Bitmap<T>::paint () {
    if ((T*)stage == data) return; // Bitmap<Color>
    for (int x=0; x<w(); ++x)
      for (int y=0; y<h(); ++y)
        stage[x+stride*y] = at(coo(x,y));
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
}

#endif
