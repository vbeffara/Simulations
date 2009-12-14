
/// @file Image.cpp
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
  Image::Image (int wd, int ht, int dp, const std::string &tit) : 
    AutoWindow(wd,ht,tit), depth(dp), D (255 / ((1<<depth)-1)) {

      if ((depth!=1)&&(depth!=2)&&(depth!=4)&&(depth!=8)) {
        std::cerr << "libvb : error : invalid depth (only 1, 2, 4 and 8 bpp allowed)." << std::endl;
        exit(1);
      }
    }

  Color Image::lazy_eval (coloring *f, int x, int y) {
    Color tmp = stage[x+stride*y];

    if (tmp == Color(0,0,0,0)) {
      tmp = f(x,y);
      putpoint (x,y,tmp);
    }

    return tmp;
  }

  void Image::tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax) {
    Color tmp = lazy_eval (f,xmin,ymin);
    bool mono = true;

    for (int i=xmin; i<=xmax; ++i) {
      if (lazy_eval (f,i,ymin) != tmp) mono=false;
      if (lazy_eval (f,i,ymax) != tmp) mono=false;
    }

    for (int j=ymin; j<=ymax; ++j) {
      if (lazy_eval (f,xmin,j) != tmp) mono=false;
      if (lazy_eval (f,xmax,j) != tmp) mono=false;
    }

    if (mono) {
      for (int i=xmin+1; i<xmax; ++i)
        for (int j=ymin+1; j<ymax; ++j)
          putpoint (i,j,tmp,0);
    } else if ((xmax-xmin) > max (ymax-ymin, 1)) {
      int xmed = (xmin+xmax)>>1;
      tessellate (f,xmin,ymin,xmed,ymax);
      tessellate (f,xmed,ymin,xmax,ymax);
    } else if (ymax>ymin+1) {
      int ymed = (ymin+ymax)>>1;
      tessellate (f,xmin,ymin,xmax,ymed);
      tessellate (f,xmin,ymed,xmax,ymax);
    }
  }
}
