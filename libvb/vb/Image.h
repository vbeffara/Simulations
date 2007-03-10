
/** @file Image.h
 * Definitions for the vb::Image class
 */

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <vb/vb_config.h>

#include <iostream>
#include <string>

#ifdef LIBVB_HAVE_SDL
#include <SDL.h>
#else
#ifndef X_WINDOW_MISSING
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#endif
#endif

namespace vb {

  /** Helper type for use in vb::Image::tessellate and
   * vb::Image::lazy_eval.
   */

  typedef char coloring (int,int);
  
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
    int pitch;           ///< The size of a line in bytes. Should be protected - don't use.
                         ///< @todo Make vb::Image::pitch protected.
    int depth;           ///< The depth of the image, in bits per pixel (1, 2 or 4).
    double outputsize;   ///< The size of the EPS output, in centimeters (0.0 to disable).
    
    int is_onscreen;     ///< 1 if the image is displayed, 0 if not.
    int cropped;         ///< 1 if the output routine should crop the picture.

    int snapshot_period; ///< The number of seconds between BMP snapshots.
    int snapshot_number; ///< The order number of the next snapshot.
    int snapshot_next;   ///< The time() of the next snapshot.
    
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
     *
     * If no display driver is set, it does nothing.
     *
     * If the display driver is SDL (default), it may change the value
     * of Image::pitch (typically to the next multiple of 4).
     */
    
    int onscreen ();

    /** Set the color of a point in the image.
     *
     * If the previous color was different and dt is set to 1 (default),
     * then it increments the clock and takes care of updating the
     * screen representation approximately 25 times per second.
     */

    int putpoint (int x, int y, int c, int dt=1) {
      int xy = x+y*pitch;
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

    /** Forcefully update the screen representation of the image.
     *
     * Bypass the auto-update feature (e.g. before a long computation).
     * To do everything by hand, use this in combination with the dt=0
     * version of Image::putpoint.
     *
     * When using the SDL display driver, also take care of keypresses,
     * with the following meanings:
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
      return pic[x+pitch*y];
    };

    /** Return the color of the image at point (xy%pitch,xy/pitch).
     *
     * This is slightly more efficient than using image(x,y) if the
     * value of x+pitch*y is already known.
     */

    char &operator() (int xy) const {
      return pic[xy];
    };

    /** Return a pointer to the raw image data - don't use it.
     *
     * It returns a char* corresponding to the contents of the image, to
     * be accessed at x+pitch*y for coordinates (x,y).
     *
     * CAUTION: the pointer gets invalidated if Image::onscreen() is
     * called. Be careful.
     */
    
    char * give_me_the_pic();
    
  protected:

    /** Output an EPS representation of the image to an ostream. */

    friend std::ostream &operator<< (std::ostream &os, vb::Image &img);    
    
    /** The raw image data - should be private. */

    char * pic;

  private:
    int pic_is_original;

    std::string title;

    unsigned long long npts;
    unsigned long delay;
    unsigned long timer;
    unsigned long saved_clock;
    unsigned long long nb_clock;
    int paused;

#ifdef LIBVB_HAVE_SDL
    SDL_Surface *screen;

    SDL_Event event;
    void events();
#else
#ifndef X_WINDOW_MISSING
    Display *display; 
    int screen;
    Window window;
    GC gc;
    int black,white;
    XImage *ximage;

    unsigned long palette[256];
#endif
#endif

    void cycle();
  };
 
}

#endif
