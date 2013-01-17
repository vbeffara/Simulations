/// @file
/// Definitions for the vb::CoarseImage class

#ifndef __VB_COARSEIMAGE_H
#define __VB_COARSEIMAGE_H

#include <vb/Bitmap.h>

namespace vb {
  /** A single cell of a CoarseImage.
   */

  class CoarseCell {
    public:

      /** Constructor for a cell of a given size.
       *
       * @param L The side length of the cell.
       */

      CoarseCell (int l) : fill(0), LL(l*l) { }

      /** Return the Color of the cell (proportion of white). */

      operator Color() { return fill*255/LL; }

      int fill;              ///< The number of pixels with value 1 in the cell.
      std::vector<char> sub; ///< The actual contents of the cell, if not constant.

    private:
      int LL;                ///< The number of vertices in the cell.
  };

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
   */

  class CoarseImage : public Bitmap<CoarseCell> {
    public:
      int true_width;    ///< The true width of the image, in pixels.
      int true_height;   ///< The true height of the image, in pixels.
      int L;             ///< The size of a block, in pixels.
      coo z0;            ///< The coordinates of the origin (hides that of vb::Bitmap).

      /** The standard constructor of the CoarseImage class.
       *
       * @param wd The width of the image, in pixels.
       * @param ht The height of the image, in pixels.
       * @param l The size of a block.
       * @param tit The title of the image.
       */

      CoarseImage (int wd, int ht, const std::string &tit, int l);

      void put (coo z, int c);

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

      int putpoint (int x, int y, int c, int dt=1) __attribute__ ((deprecated));

      char at (coo z) const;

      /** Return the color of the point at (x,y).
       *
       * Note: contrary to the Image::operator() version, this one is
       * not inlined.
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      char at (int x, int y) const __attribute__ ((deprecated)) { return at(coo(x,y)); }

      /** Syntactic shortcut for operator().
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      char operator() (int x, int y) const __attribute__ ((deprecated))
      { return at(coo(x,y)); }

      char operator() (coo z) const { return at(z); }

    private:
      int LL;
  };
}

#endif
