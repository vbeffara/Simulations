
/// @file CoarseImage.h
/// Definitions for the vb::CoarseImage class

#ifndef __VB_COARSEIMAGE_H
#define __VB_COARSEIMAGE_H

#include <vb/Image.h>

namespace vb {

  /** A multi-scale version of Image.
   *
   * The image is split into L by L blocks, and a unicolor block takes
   * very little space in memory. This allows for very large simulations
   * if the structure to be simulated is either smooth or very localized
   * (typically, it works very well for FPP-like processes, not so well
   * for DLA).
   *
   * The optimal choice for L depends on the model, but n^{1/3} is
   * usually a good value.
   *
   * The Image part corresponds to the on-screen representation, the
   * grey level corresponds to the local density. So:
   * - image.width and image.height are expressed in blocks;
   * - ((Image)coarseimage)(x,y) should work and return the current
   *   color of the screen pixel;
   * - you can say os<<coarseimage to get an EPS representation of the
   *   on-screen version.
   */

  class CoarseImage : public vb::Image {
    public:
      int true_width;    ///< The true width of the image, in pixels.
      int true_height;   ///< The true height of the image, in pixels.
      int L;             ///< The size of a block, in pixels.
    
      /** The standard constructor of the CoarseImage class.
       *
       * @param wd The width of the image, in pixels.
       * @param ht The height of the image, in pixels.
       * @param l The size of a block.
       * @param tit The title of the image.
       */

      CoarseImage (int wd, int ht, int l, std::string tit);
      
      /** The standard destructor of the CoarseImage class. */

      ~CoarseImage ();

      /** Set the color of the point at (x,y).
       *
       * It takes care of everything : allocate memory if the
       * corresponding block becomes bi-color, free it if it becomes
       * uniform.
       *
       * Note: contrary to the Image::putpoint version, this one is not
       * inlined.
       */

      int putpoint (int x, int y, int c, int dt=1);

      /** Return the color of the point at (x,y).
       *
       * Note: contrary to the Image::operator() version, this one is
       * not inlined.
       */

      char operator() (int x, int y) const;
    
    private:
      int LL;
      int nblocs;
    
      int *fill;
      char **sub;
  };

  CoarseImage::CoarseImage (int wd, int ht, int l, std::string title) 
    : Image (1+(wd-1)/l,1+(ht-1)/l,8,title),
      true_width(wd), true_height(ht), L(l), LL(l*l),
      fill(new int [width*height]), sub(new char* [width*height])
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
  
  int CoarseImage::putpoint (int x, int y, int c, int dt) {
    int coarse_x = x/L;
    int coarse_y = y/L;
    int coarse_xy = coarse_x + width * coarse_y;
    
    if (fill[coarse_xy] == c*LL) // Nothing to do
      return c;
    
    if (fill[coarse_xy] == (1-c)*LL) { // Need to create the block
      sub[coarse_xy] = new char[LL];
      for (int i=0; i<LL; i++)
	sub[coarse_xy][i] = 1-c;
      nblocs++;
    }

    int sub_xy = (x%L) + L * (y%L); 

    if (sub[coarse_xy][sub_xy] != c) {
      sub[coarse_xy][sub_xy] = c;
      fill[coarse_xy] += 2*c-1;
    }

    this->Image::putpoint (coarse_x,coarse_y,fill[coarse_xy]*255/LL,dt);

    if ((fill[coarse_xy]==0)||(fill[coarse_xy]==LL)) {
      delete[] sub[coarse_xy];
      sub[coarse_xy]=NULL;
      nblocs--;
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

#endif
