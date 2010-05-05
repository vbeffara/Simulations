/// @file
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

  void Image::fill (int x, int y, Color c, unsigned char adj) {
    Color in = at(x,y); if (in == c) return;
    putpoint(x,y,c,0);

    static std::vector<int> xy; xy.push_back(x); xy.push_back(y);

    while (xy.size()) {
      int j=xy.back(); xy.pop_back();
      int i=xy.back(); xy.pop_back();

      if ((adj&1)   && (i<width-1)                 && (at(i+1,j  )==in)) { xy.push_back(i+1); xy.push_back(j  ); putpoint (i+1,j  ,c,0); }
      if ((adj&2)   && (i<width-1) && (j<height-1) && (at(i+1,j+1)==in)) { xy.push_back(i+1); xy.push_back(j+1); putpoint (i+1,j+1,c,0); }
      if ((adj&4)   &&                (j<height-1) && (at(i  ,j+1)==in)) { xy.push_back(i  ); xy.push_back(j+1); putpoint (i  ,j+1,c,0); }
      if ((adj&8)   && (i>0)       && (j<height-1) && (at(i-1,j+1)==in)) { xy.push_back(i-1); xy.push_back(j+1); putpoint (i-1,j+1,c,0); }
      if ((adj&16)  && (i>0)                       && (at(i-1,j  )==in)) { xy.push_back(i-1); xy.push_back(j  ); putpoint (i-1,j  ,c,0); }
      if ((adj&32)  && (i>0)       && (j>0)        && (at(i-1,j-1)==in)) { xy.push_back(i-1); xy.push_back(j-1); putpoint (i-1,j-1,c,0); }
      if ((adj&64)  &&                (j>0)        && (at(i  ,j-1)==in)) { xy.push_back(i  ); xy.push_back(j-1); putpoint (i  ,j-1,c,0); }
      if ((adj&128) && (i<width-1) && (j>0)        && (at(i+1,j-1)==in)) { xy.push_back(i+1); xy.push_back(j-1); putpoint (i+1,j-1,c,0); }
    }
  }
}
