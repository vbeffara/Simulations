/// @file
/// Implementation of the vb::CoarseImage class

#include <vb/CoarseImage.h>

namespace vb {
  CoarseImage::CoarseImage (int wd, int ht, int l, const std::string &title) 
    : Image (1+(wd-1)/l,1+(ht-1)/l,title),
      true_width(wd), true_height(ht), L(l), LL(l*l),
      fill(width*height,0), sub(width*height,(char*)NULL)
  { }

  CoarseImage::~CoarseImage () {
    for (int i=0; i<width*height; ++i)            delete[] sub[i];
    for (unsigned int i=0; i<storage.size(); ++i) delete[] storage[i];
  }

  char * CoarseImage::claim (char color) {
    char *ret;
    if (storage.empty())
      ret = new char[LL];
    else {
      ret = storage.back();
      storage.pop_back();
    }

    for (int i=0; i<LL; i++) ret[i] = color;

    return ret;
  }

  void CoarseImage::release (char *box) {
    storage.push_back(box);
  }

  int CoarseImage::putpoint (int x, int y, int c, int dt) {
    int coarse_x = x/L;
    int coarse_y = y/L;
    int coarse_xy = coarse_x + width * coarse_y;

    if (fill[coarse_xy] == c*LL) // Nothing to do
      return c;

    if (fill[coarse_xy] == (1-c)*LL) { // Need to create the block
      sub[coarse_xy] = claim(1-c);
    }

    int sub_xy = (x%L) + L * (y%L); 

    if (sub[coarse_xy][sub_xy] != c) {
      sub[coarse_xy][sub_xy] = c;
      fill[coarse_xy] += 2*c-1;
    }

    this->Image::putpoint (coarse_x,coarse_y,fill[coarse_xy]*255/LL,dt);

    if ((fill[coarse_xy]==0)||(fill[coarse_xy]==LL)) {
      release (sub[coarse_xy]);
      sub[coarse_xy]=NULL;
    }

    return c;
  }

  char CoarseImage::at (int x, int y) const {
    int coarse_x = x/L;
    int coarse_y = y/L;
    int coarse_xy = coarse_x + width * coarse_y;

    if (fill[coarse_xy]==0) return 0;
    if (fill[coarse_xy]==LL) return 1;

    int sub_xy = (x%L) + L * (y%L); 
    return sub[coarse_xy][sub_xy];
  }
}
