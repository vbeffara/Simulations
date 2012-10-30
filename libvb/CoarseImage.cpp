/// @file
/// Implementation of the vb::CoarseImage class

#include <vb/CoarseImage.h>

namespace vb {
  CoarseImage::CoarseImage (int wd, int ht, const std::string &title, int l)
    : Bitmap<CoarseCell> (1+(wd-1)/l,1+(ht-1)/l,title,l),
      true_width(wd), true_height(ht), L(l), LL(l*l)
  { }

  CoarseImage::~CoarseImage () {
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
    if (dt) step();

    CoarseCell & d = data[(x/L) + stride*(y/L)];

    if (d.fill == c*LL) return c;

    if (d.fill == (1-c)*LL) { // Need to create the block
      d.sub = claim(1-c);
    }

    int sub_xy = (x%L) + L * (y%L);

    if (d.sub[sub_xy] != c) {
      d.sub[sub_xy] = c;
      d.fill += 2*c-1;
    }

    if ((d.fill==0)||(d.fill==LL)) {
      release (d.sub);
      d.sub = NULL;
    }

    return c;
  }

  char CoarseImage::at (int x, int y) const {
    const CoarseCell & d = data[(x/L) + stride*(y/L)];

    if (d.fill==0) return 0;
    if (d.fill==LL) return 1;

    return d.sub[(x%L) + L*(y%L)];
  }
}
