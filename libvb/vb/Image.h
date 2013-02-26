/// @file
/// Definitions for the vb::Image class

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <vb/Bitmap.h>

namespace vb {

  /** The main image class, used for all displays.
   *
   * It is a basic 2D canvas, greyscale (1, 2 or 4 bpp), with methods to
   * color individual points and to export a PNG file.
   */

  class Image : public Bitmap<Color> {
    public:

      /** The standard constructor of the Image class.
       *
       * @param wd The width in pixels.
       * @param ht The height in pixels.
       * @param tit The title of the image.
       */

      Image (int wd, int ht, const std::string &tit);

      /** Set the color of a point in the image.
       *
       * If dt is set to 1 (default), then it increments the clock and
       * takes care of updating the screen representation approximately
       * 25 times per second.
       *
       * @param x  The first coordinate of the point.
       * @param y  The second coordinate of the point.
       * @param c  The vb::Color to put there.
       * @param dt Whether to increment the clock.
       */

      void putpoint (int x, int y, Color c, int dt=1) __attribute__ ((deprecated)) {
        at(x,y) = c;
        if (dt) step();
      }

      /** Set the color of a point - with range-checking.
       *
       * dt=0 does not really make sense here - if you want speed, you
       * better range-check yourself and debug your code.
       *
       * @param x  The first coordinate of the point.
       * @param y  The second coordinate of the point.
       * @param c  The vb::Color to put there.
       * @param dt Whether to increment the clock.
       */

      void putpoint_safe (int x, int y, Color c, int dt=1) {
        if ( (x>=0) && (y>=0) && (x<w()) && (y<h()) ) {
          if (dt) put (coo(x,y),c);
          else at(x,y) = c;
        }
      }

      /** Lazy evaluation of f at point (x,y).
       *
       * - If image(x,y) is 0: compute f(x,y), set image(x,y) to this and
       *   return it;
       * - If image(x,y) is not 0, return it instead.
       *
       * @param f The function to evaluate if needed.
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       * @param data A pointer to additional data needed by the function.
       */

      virtual Color compute (int x, int y) { return 0; }

      Color lazy (int x, int y);

      void tessel (int xmin, int ymin, int xmax, int ymax);

      /** Return the color of the image at point (x,y).
       *
       * This is exactly equivalent to Image::at.
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      Color & operator() (int x, int y) { return at(x,y); };

      /** Fill a connected region in the image.
       *
       * If the color at coordinates (x,y) is already equal to c, then
       * do nothing; if not, paint its connected component (defined
       * according to adj) using color c.
       *
       * @param x   The first coordinate of the zone to fill.
       * @param y   The second coordinate of the zone to fill.
       * @param c   The color to put in the region.
       * @param adj The adjacency to use when filling (defaults to Z^2).
       */

      void fill (int x, int y, Color c, unsigned char adj = 85);

    protected:
      /// Update the contents of AutoWindow::stage (i.e., do nothing).
      virtual void paint ();
  };
}

#endif
