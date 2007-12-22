
/** @file Image.h
 * Definitions for the vb::Image class
 */

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <iostream>
#include <string>

#ifndef VB_NO_GUI
#include <fltk/Window.h>
#include <fltk/Rectangle.h>
#include <fltk/PixelType.h>
#include <fltk/draw.h>
#include <fltk/events.h>
#include <fltk/run.h>
#include <fltk/ask.h>
#endif

namespace vb {

  /** Helper type for use in vb::Image::tessellate and
   * vb::Image::lazy_eval.
   */

  typedef char coloring (int,int);
  
#ifndef VB_NO_GUI
  void Close_Window_CB (fltk::Widget* widget, void*);

  class Image;

  /** This is a custom FLTK window fo displaying an Image.
   */

  class Window : public fltk::Window {
    public:
      int size;                  ///< The total number of pixels in the image.
      Rectangle R;               ///< A rectangle filling the whole window, in which to draw.
      Image *img;                ///< Pointer to the contained image.
      unsigned char *T;          ///< Staging area for the image converted to greyscale.
      int D;                     ///< Multiplicator associated to the depth of the image.
      int P;                     ///< The width of the image.

      Window (Image *image);     ///< Standard constructor, builds a window for an image and show it.
      void draw();               ///< Fill the fltk::Rectangle R with the image contents.
      int handle(int event);     ///< Handle keyboard events such as 'q', 'x' etc.
  };
#endif

  /** The main image class, used for all displays.
   *
   * It is a basic 2D canvas, greyscale (1, 2 or 4 bpp), with methods to
   * color individual points and to export the contents of an EPS file
   * to an ostream such as std::cout.
   */

  class Image {
  public:
    int width;           ///< The width of the image, in pixels.
    int height;          ///< The height of the image, in pixels.
    int depth;           ///< The depth of the image, in bits per pixel (1, 2 or 4).
    double outputsize;   ///< The size of the EPS output, in centimeters (0.0 to disable).
    
    int is_onscreen;     ///< 1 if the image is displayed, 0 if not.
    int cropped;         ///< 1 if the output routine should crop the picture.

    /** The standard constructor of the Image class.
     *
     * @param wd The width in pixels.
     * @param ht The height in pixels.
     * @param dp The depth of the created bitmap (1, 2, 4 or 8, in bpp).
     * @param tit The title of the image.
     */

    Image (int wd, int ht, int dp, std::string tit);

    /** The standard destructor of the Image class. */

    ~Image ();

    /** Put the image on the screen.
     */
    
    int onscreen ();

    /** Set the color of a point in the image.
     *
     * If the previous color was different and dt is set to 1 (default),
     * then it increments the clock and takes care of updating the
     * screen representation approximately 25 times per second.
     */

    int putpoint (int x, int y, int c, int dt=1) {
      int xy = x+y*width;
      if (pic[xy]!=c) {
	pic[xy] = c;
    
	if (dt && is_onscreen) {
	  ++npts;
	  --timer;
	  if (timer==0) cycle();
	}
      }
      return c;
    }

    /** Set the color of a point - with range-checking.
     *
     * dt=0 does not really make sense here - if you want speed, you
     * better range-check yourself and debug your code.
     */

    int putpoint_safe (int x, int y, int c) {
      if ( (x>=0) && (y>=0) && (x<width) && (y<height) )
        return putpoint (x,y,c);
      return -1;
    }

    /** Forcefully update the screen representation of the image.
     *
     * Bypass the auto-update feature (e.g. before a long computation).
     * To do everything by hand, use this in combination with the dt=0
     * version of Image::putpoint.
     *
     * This also take care of keypresses, with the following meanings:
     *
     * - 'q' outputs the image to std::cout and exits with exit code 0;
     * - 'x' outputs nothing and exits with exit code 1;
     * - ' ' (the space bar) pauses/unpauses the program.
     *
     * All this depends on Image::update() being called ...
     */

    void update ();

    /** Lazy evaluation of f at point (x,y).
     *
     * - If image(x,y) is 0: compute f(x,y), set image(x,y) to this and
     *   return it;
     * - If image(x,y) is not 0, return it instead.
     */
    
    char lazy_eval (coloring *f, int x, int y);

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
     */

    void tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax);

    /** Return the color of the image at point (x,y).
     *
     * The implementation is trivial, and the function is inline, so you
     * should always use this - Image::pic[] is protected anyway.
     *
     * It can also be used as image(x,y)=c but be careful with that.
     */
    
    char &operator() (int x, int y) const {
      return pic[x+width*y];
    };

    /** Return the color of the image at point (xy%width,xy/width).
     *
     * This is slightly more efficient than using image(x,y) if the
     * value of x+width*y is already known.
     */

    char &operator() (int xy) const {
      return pic[xy];
    };

  protected:

    /** Output an EPS representation of the image to an ostream. */

    friend std::ostream &operator<< (std::ostream &os, vb::Image &img);    

#ifndef VB_NO_GUI
    friend class Window;
#endif
    
  private:
    char * pic;                       ///< The raw image data
    std::string title;                ///< The title of the image

    unsigned long long npts;
    unsigned long delay;
    unsigned long timer;
    unsigned long saved_clock;
    unsigned long long nb_clock;
    int paused;

    /** Do some bookkeeping related to the display window. */

    void cycle();

#ifndef VB_NO_GUI
    Window *win;        ///< The display window.
#endif
  };

  /** Return the parameter as a hexadecimal digit.
   *
   * It does bound-checking and returns a digit between 0 and F. There
   * probably is a more canonical way of doing that, but it works.
   *
   * @param i The integer to translate.
   * @return The corresponding digit as a char.
   */

  inline char trans (int i)
  {
    static char trans[17] = "0123456789ABCDEF";

    if (i<0) i=0;
    if (i>15) i=15;
    return trans[15-i];
  }
}

/** @example sample.cpp
 * A simple example of how to use the libvb library.
 *
 * It does nothing interesting, but demonstrates the vb::Image class,
 * with the vb::Image::tessellate() method and EPS creation. Here is a
 * line-by-line description of the main() function:
 *
 * @dontinclude sample.cpp
 * @skip int main
 * @until {
 * First, create an instance of vb::CL_Parser. Use it to get the value
 * of n, the size of the image (the default is 500 here):
 * @skip CLP
 * @until as_int
 * Then, create a vb::Image of this size, and display it on the screen:
 * @skip Image
 * @until onscreen
 * Fill it using the coloring function f:
 * @skipline tessellate
 * And finally, export it to std::cout as an EPS file and exit:
 * @skip <<
 * @until }
 *
 * Full source code of sample.cpp:
 */

#endif
