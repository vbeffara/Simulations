/// @file
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

namespace vb {
  Image::Image (int wd, int ht, const std::string &tit) : Bitmap<Color> (wd,ht,tit) { }

  void Image::fill (coo z, Color c, int adj) {
    Color in = at(z); if (in == c) return;

    std::vector<coo> xy;
    xy.push_back(z); at(z) = c;

    while (xy.size()) {
      coo ij = xy.back(); xy.pop_back();
      for (int d=0; d<adj; ++d) {
        coo nij = ij + dz[d];
        if (contains(nij) && (at(nij) == in)) {
          xy.push_back(nij); at(nij)=c;
        }
      }
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
          at(coo(i,j)) = tmp;
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
