/// @file CoarseImage.h
/// Definitions for the vb::CoarseImage class

#ifndef __VB_COARSEIMAGE_H
#define __VB_COARSEIMAGE_H

#include <vb/Image.h>

namespace vb {

  /** A multi-scale version of Image.
   *
   * The image is split into L by L blocks, and a unicolor block takes
   * very little space in memory. This allows for very large simulations
   * if the structure to be simulated is either smooth or very localized
   * (typically, it works very well for FPP-like processes, not so well
   * for DLA).
   *
   * The optimal choice for L depends on the model, but n^{1/3} is
   * usually a good value.
   *
   * The Image part corresponds to the on-screen representation, the
   * grey level corresponds to the local density. So:
   * - image.width and image.height are expressed in blocks;
   * - ((Image)coarseimage)(x,y) should work and return the current
   *   color of the screen pixel;
   * - you can say os<<coarseimage to get an EPS representation of the
   *   on-screen version.
   */

  class CoarseImage : public vb::Image {
    public:
      int true_width;    ///< The true width of the image, in pixels.
      int true_height;   ///< The true height of the image, in pixels.
      int L;             ///< The size of a block, in pixels.
    
      /** The standard constructor of the CoarseImage class.
       *
       * @param wd The width of the image, in pixels.
       * @param ht The height of the image, in pixels.
       * @param l The size of a block.
       * @param tit The title of the image.
       */

      CoarseImage (int wd, int ht, int l, std::string tit);
      
      /** The standard destructor of the CoarseImage class. */

      ~CoarseImage ();

      /** Set the color of the point at (x,y).
       *
       * It takes care of everything : allocate memory if the
       * corresponding block becomes bi-color, free it if it becomes
       * uniform.
       *
       * @param x  The first coordinate of the point.
       * @param y  The second coordinate of the point.
       * @param c  The color to put there.
       * @param dt Whether to increment the clock.
       */

      int putpoint (int x, int y, int c, int dt=1);

      /** Return the color of the point at (x,y).
       *
       * Note: contrary to the Image::operator() version, this one is
       * not inlined.
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      char operator() (int x, int y) const;
    
    private:
      int LL;
      int nblocs;
    
      int *fill;
      char **sub;
  };
}

#endif
