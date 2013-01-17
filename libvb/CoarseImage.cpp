/// @file
/// Implementation of the vb::CoarseImage class

#include <vb/CoarseImage.h>

namespace vb {
  CoarseImage::CoarseImage (int wd, int ht, const std::string &title, int l)
    : Bitmap<CoarseCell> (1+(wd-1)/l,1+(ht-1)/l,title,l),
      true_width(wd), true_height(ht), L(l), LL(l*l), z0(0)
  { }

  void CoarseImage::put (coo z, int c) {
    step();

    int x=real(z+z0), y=imag(z+z0);
    CoarseCell & d = Bitmap<CoarseCell>::at(x/L,y/L);

    if (d.fill == c*LL) return;

    if (d.fill == (1-c)*LL) { // Need to create the block
      d.sub = new char[LL];
      for (int i=0; i<LL; i++) d.sub[i] = 1-c;
    }

    int sub_xy = (x%L) + L * (y%L);

    if (d.sub[sub_xy] != c) {
      d.sub[sub_xy] = c;
      d.fill += 2*c-1;
    }

    if ((d.fill==0)||(d.fill==LL)) {
      delete[] d.sub;
      d.sub = NULL;
    }
  }

  int CoarseImage::putpoint (int x, int y, int c, int dt) {
    put (coo(x,y),c);
    return c;
  }

  char CoarseImage::at (coo z) const {
    int x=real(z+z0), y=imag(z+z0);
    const CoarseCell & d = data[(x/L) + stride*(y/L)];

    if (d.fill==0) return 0;
    if (d.fill==LL) return 1;

    return d.sub[(x%L) + L*(y%L)];
  }
}
