/// @file
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
  Image::Image (int wd, int ht, const std::string &tit) : Bitmap<Color> (wd,ht,tit) { }

  void Image::fill (int x, int y, Color c, unsigned char adj) {
    Color in = at(coo(x,y)); if (in == c) return;
    at(x,y) = c;

    static std::vector<int> xy; xy.push_back(x); xy.push_back(y);

    while (xy.size()) {
      int j=xy.back(); xy.pop_back();
      int i=xy.back(); xy.pop_back();

      if ((adj&1)   && (i<w()-1)              && (at(i+1,j  )==in)) { xy.push_back(i+1); xy.push_back(j  ); at (i+1,j)   = c; }
      if ((adj&2)   && (i<w()-1) && (j<h()-1) && (at(i+1,j+1)==in)) { xy.push_back(i+1); xy.push_back(j+1); at (i+1,j+1) = c; }
      if ((adj&4)   &&              (j<h()-1) && (at(i  ,j+1)==in)) { xy.push_back(i  ); xy.push_back(j+1); at (i  ,j+1) = c; }
      if ((adj&8)   && (i>0)     && (j<h()-1) && (at(i-1,j+1)==in)) { xy.push_back(i-1); xy.push_back(j+1); at (i-1,j+1) = c; }
      if ((adj&16)  && (i>0)                  && (at(i-1,j  )==in)) { xy.push_back(i-1); xy.push_back(j  ); at (i-1,j)   = c; }
      if ((adj&32)  && (i>0)     && (j>0)     && (at(i-1,j-1)==in)) { xy.push_back(i-1); xy.push_back(j-1); at (i-1,j-1) = c; }
      if ((adj&64)  &&              (j>0)     && (at(i  ,j-1)==in)) { xy.push_back(i  ); xy.push_back(j-1); at (i  ,j-1) = c; }
      if ((adj&128) && (i<w()-1) && (j>0)     && (at(i+1,j-1)==in)) { xy.push_back(i+1); xy.push_back(j-1); at (i+1,j-1) = c; }
    }
  }

  Color Image::lazy (coo z) {
    if (int(at(z)) == 0) put(z, compute(z));
    return at(z);
  }

  void Image::tessel (int xmin, int ymin, int xmax, int ymax) {
    Color tmp = lazy (coo(xmin,ymin));
    bool mono = true;

    for (int i=xmin; i<=xmax; ++i) {
      if (lazy (coo(i,ymin)) != tmp) mono=false;
      if (lazy (coo(i,ymax)) != tmp) mono=false;
    }

    for (int j=ymin; j<=ymax; ++j) {
      if (lazy (coo(xmin,j)) != tmp) mono=false;
      if (lazy (coo(xmax,j)) != tmp) mono=false;
    }

    if (mono) {
      for (int i=xmin+1; i<xmax; ++i)
        for (int j=ymin+1; j<ymax; ++j)
          put (coo(i,j), tmp);
    } else if ((xmax-xmin) > std::max (ymax-ymin, 1)) {
      int xmed = (xmin+xmax)>>1;
      tessel (xmin,ymin,xmed,ymax);
      tessel (xmed,ymin,xmax,ymax);
    } else if (ymax>ymin+1) {
      int ymed = (ymin+ymax)>>1;
      tessel (xmin,ymin,xmax,ymed);
      tessel (xmin,ymed,xmax,ymax);
    }
  }
}
