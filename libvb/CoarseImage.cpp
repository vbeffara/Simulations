
/// @file CoarseImage.cpp
/// Implementation of the vb::CoarseImage class

#include <vb/CoarseImage.h>

namespace vb {
  CoarseImage::CoarseImage (int wd, int ht, int l, std::string title) 
    : Image (1+(wd-1)/l,1+(ht-1)/l,8,title),
      true_width(wd), true_height(ht), L(l), LL(l*l),
      fill(new int [width*height]), sub(new char* [width*height]),
      stored(0)
  {
    for (int i=0; i<width*height; i++) {
      fill[i] = 0;
      sub[i] = NULL;
    }
  }    
  
  CoarseImage::~CoarseImage () {
    for (int i=0; i<width*height; i++)
      delete[] sub[i];
    delete[] sub;
    delete[] fill;
  }

  char * CoarseImage::claim (char color) {
    char *ret;
    if (stored) { ret=storage[--stored]; }
    else ret = new char[LL];

    if (ret[0] != color) {
      for (int i=0; i<LL; i++)
        ret[i] = color;
    }
    return ret;
  }

  void CoarseImage::release (char *box) {
    if (stored == storage.size()) storage.push_back(box);
    else storage[stored] = box;
    ++stored;
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

  char CoarseImage::operator() (int x, int y) const {
    int coarse_x = x/L;
    int coarse_y = y/L;
    int coarse_xy = coarse_x + width * coarse_y;
  
    if (fill[coarse_xy]==0) return 0;
    if (fill[coarse_xy]==LL) return 1;

    int sub_xy = (x%L) + L * (y%L); 
    return sub[coarse_xy][sub_xy];
  }
}
