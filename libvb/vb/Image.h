
/** @file Image.h
 * Definitions for the vb::Image class
 */

#ifndef __VB_IMAGE_H
#define __VB_IMAGE_H

#include <iostream>
#include <string>

#include <SDL.h>

#include <fltk/Window.H>
#include <fltk/Rectangle.H>
#include <fltk/PixelType.H>
#include <fltk/draw.H>
#include <fltk/run.H>

namespace vb {

  /** Helper type for use in vb::Image::tessellate and
   * vb::Image::lazy_eval.
   */

  typedef char coloring (int,int);
  
  /** This is a custom FLTK window fo displaying an Image.
   */

  class Window : public fltk::Window {
    public:
      Rectangle R;
      char *S;
      int P;

      Window (int wd, int ht, const char *title, char *pic, int pitch) :
            fltk::Window(wd,ht,title), S(pic), P(pitch) {
        begin();
          R = Rectangle (0,0,wd,ht);
        end();
        show();
      }

      void draw() {
        drawimage ((unsigned char *)S,fltk::MONO,R,P);
      }
  };

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

    SDL_Surface *screen;

    SDL_Event event;
    void events();

    void cycle();

    Window *win;
  };
 
  Image::Image (int wd, int ht, int dp, std::string tit)
    : width(wd), height(ht), pitch(wd), depth(dp), outputsize(0.0),
    is_onscreen(0), cropped(0), snapshot_period(0), snapshot_number(0),
    snapshot_next(0), pic_is_original(1), title(tit), npts(0), delay(1),
    timer(1), saved_clock(clock()), nb_clock(0), paused(0) { 

      if ((depth!=1)&&(depth!=2)&&(depth!=4)&&(depth!=8)) {
        std::cerr << "libvb : error : invalid depth"
          << " (only 1, 2, 4 and 8 bpp allowed).\n";
        exit(1);
      }

      pic = new char[width*ht];
      for (int i=0; i<width*ht; i++)
        pic[i]=0;

      if (!(pic)) {
        std::cerr << "libvb : error : image too large.\n";
        delete[] pic;
        exit (1);
      }
    }

  Image::~Image () {
    if (pic_is_original)
      delete[] pic;
  }

  char * Image::give_me_the_pic() {
    return pic;
  }

  void Image::cycle () {
    long tmp = clock() - saved_clock;
    if (tmp>=0) nb_clock += tmp+1;
    if (nb_clock < CLOCKS_PER_SEC/5)
      delay *= 2;
    else {
      delay = 1 + npts * (CLOCKS_PER_SEC/25) / nb_clock;
      update();
    }
    timer = delay;
    saved_clock = clock();
  }

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

  /** Outputs an image to an output stream.
   *
   * Nuff said.
   *
   * @param os The output stream.
   * @param img The image to output.
   * @return The same output stream.
   */

  std::ostream &operator<< (std::ostream &os, Image &img) {
    long bits;
    long xmin,xmax,ymin,ymax;
    long out_wd, out_ht, out_max;

    img.update();

    int x=0,y=0,dx=img.width,dy=img.height;

    int white = (1<<img.depth)-1;

    if (img.cropped) {
      xmin=x+dx-1; ymin=y+dy-1; xmax=x; ymax=y;
      for (int i=x;i<x+dx;i++)
        for (int j=y;j<y+dy;j++)
          if (img(i,j)&white) {
            if (i<xmin) xmin=i;
            if (i>xmax) xmax=i;
            if (j<ymin) ymin=j;
            if (j>ymax) ymax=j;
          }
      x=xmin; dx=xmax-xmin+1; y=ymin; dy=ymax-ymin+1;
    }

    bits = dx * img.depth;

    if (img.outputsize == 0.0) {
      out_wd = dx;
      out_ht = dy;
    } else {
      if (dx>dy) out_max=dx; 
      else out_max=dy;
      out_wd = (long) ( img.outputsize * 72.0 / 2.54 * (double)dx / out_max );
      out_ht = (long) ( img.outputsize * 72.0 / 2.54 * (double)dy / out_max );
    }

    /* EPS header */

    os << "%!PS-Adobe-2.0 EPSF-2.0\n";
    os << "%%\n";
    os << "%%Title: " << img.title << "\n";
    os << "%%\n";
    os << "%%Creator: libvb - © 2001-2007 VB - GPL\n";
    os << "%%Creator:   Mail: Vincent.Beffara@ens-lyon.fr\n";
    os << "%%Creator:   Web:  <http://www.umpa.ens-lyon.fr/~vbeffara/>\n";
    os << "%%\n";
    os << "%%BoundingBox: 0 0 " << out_wd << " " << out_ht << "\n\n";

    /* Commands */

    os << "save 20 dict begin /xpixels " 
      << dx << " def /ypixels " << dy << " def\n";
    os << "/pix " << (bits+7)/8 << " string def "
      << out_wd << " " << out_ht << " scale\n";
    os << "xpixels ypixels " << img.depth << " [xpixels 0 0 ypixels 0 0]\n";
    os << "{currentfile pix readhexstring pop} image\n";

    /* Image */

    int step = 8/img.depth;
    
    for (int j=0; j<dy; ++j) {
      int i;
      for (i=0; i<dx; i+=step) {
        int acc = 0;
        for (int k=0; k<step; ++k) {
          acc <<= img.depth;
          if (i+k<dx) acc += img(x+i+k,y+j)&white;
          else acc += white;
        }
        os << trans(acc>>4) << trans(acc&15);
        if ((i/step)%32==31) os << std::endl;
      }
      if ((i/step)%32!=0) os << std::endl;
    }

    /* End of file */

    os << "end restore\n";

    return os;
  }

  char Image::lazy_eval (coloring *f, int x, int y)
  {
    char tmp = (*this)(x,y);

    if (tmp==0) 
      putpoint (x,y,tmp=f(x,y));

    return tmp;
  }

  void Image::tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax)
  {
    int left,right,top,down;
    int i,j,xmed,ymed;

    left = lazy_eval (f,xmin,ymin); down=left;
    right = lazy_eval (f,xmax,ymax); top=right;

    for (i=xmin;i<=xmax;i++) {
      if (lazy_eval (f,i,ymin) != down) down=0;
      if (lazy_eval (f,i,ymax) != top) top=0;
    }

    for (i=ymin;i<=ymax;i++) {
      if (lazy_eval (f,xmin,i) != left) left=0;
      if (lazy_eval (f,xmax,i) != right) right=0;
    }

    if ((xmax-xmin>1)&&(ymax-ymin>1)) {
      if (left&&right&&top&&down) {
        for (i=xmin+1;i<xmax;i++) {
          for (j=ymin+1;j<ymax;j++) {
            putpoint (i,j,left,0);
          }
        }
      } else if ((xmax-xmin)>(ymax-ymin)) {
        xmed = (xmin+xmax)>>1;
        tessellate (f,xmin,ymin,xmed,ymax);
        tessellate (f,xmed,ymin,xmax,ymax);
      } else {
        ymed = (ymin+ymax)>>1;
        tessellate (f,xmin,ymin,xmax,ymed);
        tessellate (f,xmin,ymed,xmax,ymax);
      }
    }
  }

  void Image::events () {
    while (( (paused==0)&&SDL_PollEvent(&event) ) ||
	   ( (paused==1)&&SDL_WaitEvent(&event) )) {
      switch (event.type) {
      case SDL_QUIT:
	exit (1);
	break;
      case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	case SDLK_q:
	  std::cout << (*this);
	  exit (0);
	  break;
        case SDLK_x:
          exit (1);
          break;
	case SDLK_SPACE:
	  paused = 1-paused;
	  break;
	default:
	  break;
	}
	break;
      }
    }
  }
  
  int Image::onscreen () {
    SDL_Init(SDL_INIT_VIDEO);
    
    screen=SDL_SetVideoMode(width,height,8,SDL_SWSURFACE);
    if (!screen) {
      std::cerr << "libvb : error : "
		<< "Couldn't map it ! Continuing without.\n";
      return 0;
    }
    
    SDL_WM_SetCaption (title.c_str(),"Simulation");
    
    SDL_Color sdl_palette[256];
    
    int pstep = 255 / ((1<<depth)-1);
    for (int i=0;i<(1<<depth);i++) {
      sdl_palette[i].r = i*pstep;
      sdl_palette[i].g = i*pstep;
      sdl_palette[i].b = i*pstep;
    }    

    SDL_SetColors (screen,sdl_palette,0,1<<depth);

    is_onscreen = 1;
    
    char *bufp = (char*) screen->pixels;
    for (int i=0;i<width;i++)
      for (int j=0;j<height;j++)
	bufp[i+screen->pitch*j] = pic[i+pitch*j];

    delete[] pic;
    pic = bufp;
    pic_is_original = 0;
    pitch = screen->pitch;

    win = new Window (width,height,title.c_str(),pic,pitch);

    update();  
    
    return 1;
  }
  
  void Image::update () {
    if (is_onscreen) {
      SDL_UpdateRect(screen,0,0,0,0);
      win->redraw();

      if ( (snapshot_period>0) && (time(0)>=snapshot_next) ) {
        char buffer[100];
        sprintf (buffer,"snapshot_%06d.bmp",snapshot_number++);
        SDL_SaveBMP (screen,buffer);
        snapshot_next = time(0) + snapshot_period;
      }

      events();
      fltk::check();
    }
  }  
}

#endif
