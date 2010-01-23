
/// @file Image.cpp
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
  Image::Image (int wd, int ht, const std::string &tit) : AutoWindow(wd,ht,tit) { }

  Color Image::lazy_eval (coloring *f, int x, int y, void *data) {
    Color tmp = stage[x+stride*y];

    if (tmp == Color(0,0,0,0)) {
      tmp = f(x,y,data);
      putpoint (x,y,tmp);
    }

    return tmp;
  }

  void Image::tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax, void *data) {
    Color tmp = lazy_eval (f,xmin,ymin,data);
    bool mono = true;

    for (int i=xmin; i<=xmax; ++i) {
      if (lazy_eval (f,i,ymin,data) != tmp) mono=false;
      if (lazy_eval (f,i,ymax,data) != tmp) mono=false;
    }

    for (int j=ymin; j<=ymax; ++j) {
      if (lazy_eval (f,xmin,j,data) != tmp) mono=false;
      if (lazy_eval (f,xmax,j,data) != tmp) mono=false;
    }

    if (mono) {
      for (int i=xmin+1; i<xmax; ++i)
        for (int j=ymin+1; j<ymax; ++j)
          putpoint (i,j,tmp,0);
    } else if ((xmax-xmin) > max (ymax-ymin, 1)) {
      int xmed = (xmin+xmax)>>1;
      tessellate (f,xmin,ymin,xmed,ymax,data);
      tessellate (f,xmed,ymin,xmax,ymax,data);
    } else if (ymax>ymin+1) {
      int ymed = (ymin+ymax)>>1;
      tessellate (f,xmin,ymin,xmax,ymed,data);
      tessellate (f,xmin,ymed,xmax,ymax,data);
    }
  }

  void Image::fill (int x, int y, Color in, Color out, unsigned char adj) {
    if (at(x,y) != in) return;

    std::vector<int> fifox(4*width*height);
    std::vector<int> fifoy(4*width*height);

    long imin,imax;

    imin=0; imax=0;
    fifox[0]=x; fifoy[0]=y;
    putpoint(x,y,out); 

    while (imin<=imax) {
      int i=fifox[imin];
      int j=fifoy[imin];
      int k=i+j*stride;
      imin++;
      if ((i<width-1)&&(at(i+1,j)==in)) {
        fifox[++imax]=i+1;
        fifoy[imax]=j;
        putpoint (i+1,j,out);
      }
      if ((i>0)&&(at(k-1)==in)) {
        fifox[++imax]=i-1;
        fifoy[imax]=j;
        putpoint (i-1,j,out);
      }
      if ((j<height-1)&&(at(k+stride)==in)) {
        fifox[++imax]=i;
        fifoy[imax]=j+1;
        putpoint (i,j+1,out);
      }
      if ((j>0)&&(at(k-stride)==in)) {
        fifox[++imax]=i;
        fifoy[imax]=j-1;
        putpoint (i,j-1,out);
      }
    }  
  }
}
