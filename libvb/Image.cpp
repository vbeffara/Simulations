
/// @file Image.cpp
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
#ifdef LIBVB_FLTK
  void Close_Window_CB (fltk::Widget *widget, void *nothing) {
    if (fltk::ask("Do you really want to exit?"))
      exit(1);
  }
#endif

  Image::Image (int wd, int ht, int dp, std::string tit) :
#ifdef LIBVB_FLTK
    fltk::Window(wd,ht,tit.c_str()), stage(NULL),
#endif
    width(wd), height(ht), depth(dp), outputsize(0.0),
    cropped(0), title(tit), npts(0), delay(1), timer(1), 
    saved_clock(clock()), nb_clock(0), paused(0) { 

      if ((depth!=1)&&(depth!=2)&&(depth!=4)&&(depth!=8)) {
        std::cerr << "libvb : error : invalid depth"
          << " (only 1, 2, 4 and 8 bpp allowed).\n";
        exit(1);
      }

      pic = new char[width*ht];
      if (!(pic)) {
        std::cerr << "libvb : error : image too large.\n";
        delete[] pic;
        exit (1);
      }

      for (int i=0; i<width*ht; i++)
        pic[i]=0;
    }

  Image::~Image () {
    delete[] pic;
#ifdef LIBVB_FLTK
    if ((depth<8) && (stage != NULL)) delete[] stage;
#endif
  }

  void Image::cycle () {
#ifdef LIBVB_FLTK
    long tmp = clock() - saved_clock;
    if (tmp>=0) nb_clock += tmp+1;
    if (nb_clock < CLOCKS_PER_SEC/5)
      delay *= 2;
    else {
      delay = 1 + npts * (CLOCKS_PER_SEC/20) / nb_clock;
      update();
    }
    timer = delay;
    saved_clock = clock();
#endif
  }

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

    if (tmp==0) {
      tmp = f(x,y);
      putpoint (x,y,tmp);
    }

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

  void Image::show () {
#ifdef LIBVB_FLTK
    if (stage == NULL) {
      if (depth<8) stage = (unsigned char *) malloc (width*height*sizeof(unsigned char));
      else stage = (unsigned char *) pic;
      begin();
        R = Rectangle (0,0,width,height);
      end();
      callback(Close_Window_CB);
    }

    Window::show();
    update();  
#endif
  }
  
  void Image::update () {
#ifdef LIBVB_FLTK
    if (visible()) {
      redraw();
      fltk::check();
      while (paused) fltk::wait();
    }
#endif
  }  

#ifdef LIBVB_FLTK
  void Image::draw() {
    if (depth<8) {
      char D = 255 / ((1<<depth)-1);
      for (int i=0; i<width*height; ++i) stage[i] = D * pic[i];
    }
    drawimage (stage,fltk::MONO,R,width);
  }

  int Image::handle (int event) {
    switch (event) {
      case fltk::KEY:
        switch (fltk::event_key()) {
          case fltk::EscapeKey:
          case 0x61:             // this is a A (AZERTY for Q)
          case 0x71:             // this is a Q
            std::cout << (*this);
            exit (0);
            break;
          case 0x78:             // this is an X
            exit (1);
            break;
          case fltk::SpaceKey:
            paused = 1-paused;
            break;
        }
        break;
    }

    return 1;
  }
#endif
}
