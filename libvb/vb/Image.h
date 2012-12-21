/// @file
/// Definitions for the vb::Image class

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <vb/Bitmap.h>

namespace vb {
  /** Helper type for use in vb::Image::tessellate and
   * vb::Image::lazy_eval.
   */

  typedef Color coloring (int,int,void*);

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

      void putpoint (int x, int y, Color c, int dt=1) {
        stage[x+stride*y] = c;
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
        if ( (x>=0) && (y>=0) && (x<w()) && (y<h()) )
          putpoint (x,y,c,dt);
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

      void tessel (int xmin, int ymin, int xmax, int xmax);

      Color lazy_eval (coloring *f, int x, int y, void *data = NULL);

      /** Compute the color of each point using a dyadic algorithm.
       *
       * Ideally, the image after this satisfies image(x,y)==f(x,y). It
       * assumes that the initial non-0 colors are correct, and may miss
       * some details.
       *
       * One case where it is guaranteed to work is if every connected
       * component of the target image touches the boundary - e.g. if
       * the image represents the 2 sides of a Jordan curve, such as in
       * SLE.cpp.
       *
       * @param f    The coloring function.
       * @param xmin The first coordinate of the top-left corner.
       * @param ymin The second coordinate of the top-left corner.
       * @param xmax The first coordinate of the bottom-right corner.
       * @param ymax The second coordinate of the bottom-right corner.
       * @param data A pointer to additional data needed by the coloring function.
       */

      void tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax, void *data = NULL);

      /** Return the color of the image at point (x,y).
       *
       * This is exactly equivalent to Image::at.
       *
       * @param x The first coordinate of the point.
       * @param y The second coordinate of the point.
       */

      Color & operator() (int x, int y=0) { return at(x,y); };

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

/** @example sample.cpp
 * A simple example of how to use the libvb library.
 *
 * It does nothing interesting, but demonstrates the vb::Image class,
 * with the vb::Image::tessellate() method and PNG creation. Here is a
 * line-by-line description of the main() function:
 *
 * @dontinclude sample.cpp
 * @skip int main
 * @until {
 * First, create an instance of vb::CL_Parser. Use it to get the value
 * of n, the size of the image (the default is 500 here):
 * @skip CLP
 * @until CLP(
 * Then, create a vb::Image of this size, and display it on the screen:
 * @skip Image
 * @until show
 * Fill it using the coloring function f:
 * @skipline tessellate
 * And finally, export it to 'output.png' (automatic) and exit:
 * @skip <<
 * @until }
 *
 * Full source code of sample.cpp:
 */

#endif
