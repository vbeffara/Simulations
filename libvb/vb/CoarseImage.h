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
      CoarseCell (int _L) : fill(0), L(_L), LL(L*L) { }
      operator Color() { return fill*255/LL; }
      int fill;
    private:
      int L,LL;
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

      /** The standard constructor of the CoarseImage class.
       *
       * @param wd The width of the image, in pixels.
       * @param ht The height of the image, in pixels.
       * @param l The size of a block.
       * @param tit The title of the image.
       */

      CoarseImage (int wd, int ht, int l, const std::string &tit);

      /** The standard destructor of the CoarseImage class. */

      ~CoarseImage ();

      /** Produce a sub_box of appropriate size.
       *
       * We re-use old boxes to get better efficiency, it saves about 
       * 10% I guess.
       *
       * @param color The color to fill the box with (0 or 1).
       */

      char * claim (char color);

      /** Get rid of a full box.
       *
       * Rather than delete[]ing it, we save it for later use, that's a 
       * little bit faster given that the boxes are all of the same 
       * size. Still need to put NULL where it was to avoid double 
       * deletes in the destructor.
       *
       * @param box The box to get rid of.
       */

      void release (char * box);

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

      char at (int x, int y) const;

      /** Syntactic shortcut for operator().
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      char operator() (int x, int y) const { return at(x,y); }

    private:
      int LL;

      std::vector <char *> sub;
      std::vector <char *> storage;
  };
}

#endif
