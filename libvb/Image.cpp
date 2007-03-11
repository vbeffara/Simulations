
#include <vb/Image.h>

namespace vb {

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
      for (int i=0; i<dx; i+=step) {
        int acc = 0;
        for (int k=0; k<step; ++k) {
          acc <<= img.depth;
          if (i+k<dx) acc += img(x+i+k,y+j)&white;
          else acc += white;
        }
        os << trans(acc>>4) << trans(acc&15);
        if ((i/step)%32==31) os << std::endl;
      }
      if ((dx/step)%32!=0) os << std::endl;
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

    update();  
    
    return 1;
  }
  
  void Image::update () {
    if (is_onscreen) {
      SDL_UpdateRect(screen,0,0,0,0);
      if ( (snapshot_period>0) && (time(0)>=snapshot_next) ) {
        char buffer[100];
        sprintf (buffer,"snapshot_%06d.bmp",snapshot_number++);
        SDL_SaveBMP (screen,buffer);
        snapshot_next = time(0) + snapshot_period;
      }
      events();
    }
  }  
}
