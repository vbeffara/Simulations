
/// @file Image.cpp
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
  inline char trans (int i) {
    static char trans[17] = "0123456789ABCDEF";

    if (i<0) i=0;
    if (i>15) i=15;
    return trans[15-i];
  }

  Image::Image (int wd, int ht, int dp, const std::string &tit) : 
    AutoWindow(wd,ht,tit), width(wd), height(ht), depth(dp), 
    cropped(0), pic(stride*ht,0), title(tit) { 

      if ((depth!=1)&&(depth!=2)&&(depth!=4)&&(depth!=8)) {
        std::cerr << "libvb : error : invalid depth"
          << " (only 1, 2, 4 and 8 bpp allowed).\n";
        exit(1);
      }
    }

  Image::~Image () {
    std::ostringstream s;
    s << title << ".png";
    output_png (s.str());
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

  void Image::tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax) {
    int left,right,top,down;
    int i,j,xmed,ymed;

    left =  lazy_eval (f,xmin,ymin); down=left;
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

  void Image::compute_stage () {
    char D = 255 / ((1<<depth)-1);

    for (int i=0; i<width; ++i) {
      for (int j=0; j<height; ++j) {
        unsigned char c = D * pic[i+width*j];
        for (int k=0; k<4; ++k) stage[4*i+stride*j+k] = c;
      }
    }

    for (int i=0; i<width*height; ++i) stage[4*i] = D * pic[i];
  }

  unsigned char * Image::image_data () { 
    compute_stage();
    return stage;
  }

#ifdef HAVE_FLTK
  void Image::draw() {
    compute_stage();
    fl_draw_image_mono (stage,0,0,width,height,4,stride);
  }
#endif

  void Image::paint () { }
}
