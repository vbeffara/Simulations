
/// @file Image.cpp
/// Implementation of the vb::Image and vb::Window classes.

#include <vb/Image.h>

#ifdef LIBVB_FLTK
#include <fltk/draw.h>
#endif

namespace vb {
  Image::Image (int wd, int ht, int dp, std::string tit) : 
    AutoWindow(wd,ht,tit), width(wd), height(ht), depth(dp), 
    outputsize(0.0), cropped(0), title(tit) { 

      if ((depth!=1)&&(depth!=2)&&(depth!=4)&&(depth!=8)) {
        std::cerr << "libvb : error : invalid depth"
          << " (only 1, 2, 4 and 8 bpp allowed).\n";
        exit(1);
      }

      pic = new char[width*ht];
      if (!(pic)) {
        std::cerr << "libvb : error : image too large.\n";
        delete[] pic;
        exit (1);
      }

      for (int i=0; i<width*ht; i++)
        pic[i]=0;

#ifdef LIBVB_FLTK
      stage = NULL;
#endif
    }

  Image::~Image () {
    delete[] pic;
#ifdef LIBVB_FLTK
    if ((depth<8) && (stage != NULL)) delete[] stage;
#endif
  }

  std::ostream &operator<< (std::ostream &os, Image &img) {
    long bits;
    long xmin,xmax,ymin,ymax;
    long out_wd, out_ht, out_max;

    img.update();

    int x=0,y=0,dx=img.width,dy=img.height;

    int white = (1<<img.depth)-1;

    if (img.cropped) {
      xmin=x+dx-1; ymin=y+dy-1; xmax=x; ymax=y;
      for (int i=x;i<x+dx;i++)
        for (int j=y;j<y+dy;j++)
          if (img(i,j)&white) {
            if (i<xmin) xmin=i;
            if (i>xmax) xmax=i;
            if (j<ymin) ymin=j;
            if (j>ymax) ymax=j;
          }
      x=xmin; dx=xmax-xmin+1; y=ymin; dy=ymax-ymin+1;
    }

    bits = dx * img.depth;

    if (img.outputsize == 0.0) {
      out_wd = dx;
      out_ht = dy;
    } else {
      if (dx>dy) out_max=dx; 
      else out_max=dy;
      out_wd = (long) ( img.outputsize * 72.0 / 2.54 * (double)dx / out_max );
      out_ht = (long) ( img.outputsize * 72.0 / 2.54 * (double)dy / out_max );
    }

    /* EPS header */

    os << "%!PS-Adobe-2.0 EPSF-2.0\n";
    os << "%%\n";
    os << "%%Title: " << img.title << "\n";
    os << "%%\n";
    os << "%%Creator: libvb - © 2001-2007 VB - GPL\n";
    os << "%%Creator:   Mail: Vincent.Beffara@ens-lyon.fr\n";
    os << "%%Creator:   Web:  <http://www.umpa.ens-lyon.fr/~vbeffara/>\n";
    os << "%%\n";
    os << "%%BoundingBox: 0 0 " << out_wd << " " << out_ht << "\n\n";

    /* Commands */

    os << "save 20 dict begin /xpixels " 
      << dx << " def /ypixels " << dy << " def\n";
    os << "/pix " << (bits+7)/8 << " string def "
      << out_wd << " " << out_ht << " scale\n";
    os << "xpixels ypixels " << img.depth << " [xpixels 0 0 ypixels 0 0]\n";
    os << "{currentfile pix readhexstring pop} image\n";

    /* Image */

    int step = 8/img.depth;
    
    for (int j=0; j<dy; ++j) {
      int i;
      for (i=0; i<dx; i+=step) {
        int acc = 0;
        for (int k=0; k<step; ++k) {
          acc <<= img.depth;
          if (i+k<dx) acc += img(x+i+k,y+j)&white;
          else acc += white;
        }
        os << trans(acc>>4) << trans(acc&15);
        if ((i/step)%32==31) os << std::endl;
      }
      if ((i/step)%32!=0) os << std::endl;
    }

    /* End of file */

    os << "end restore\n";

    return os;
  }

  char Image::lazy_eval (coloring *f, int x, int y)
  {
    char tmp = (*this)(x,y);

    if (tmp==0) {
      tmp = f(x,y);
      putpoint (x,y,tmp);
    }

    return tmp;
  }

  void Image::tessellate (coloring *f, int xmin, int ymin, int xmax, int ymax)
  {
    int left,right,top,down;
    int i,j,xmed,ymed;

    left = lazy_eval (f,xmin,ymin); down=left;
    right = lazy_eval (f,xmax,ymax); top=right;

    for (i=xmin;i<=xmax;i++) {
      if (lazy_eval (f,i,ymin) != down) down=0;
      if (lazy_eval (f,i,ymax) != top) top=0;
    }

    for (i=ymin;i<=ymax;i++) {
      if (lazy_eval (f,xmin,i) != left) left=0;
      if (lazy_eval (f,xmax,i) != right) right=0;
    }

    if ((xmax-xmin>1)&&(ymax-ymin>1)) {
      if (left&&right&&top&&down) {
        for (i=xmin+1;i<xmax;i++) {
          for (j=ymin+1;j<ymax;j++) {
            putpoint (i,j,left,0);
          }
        }
      } else if ((xmax-xmin)>(ymax-ymin)) {
        xmed = (xmin+xmax)>>1;
        tessellate (f,xmin,ymin,xmed,ymax);
        tessellate (f,xmed,ymin,xmax,ymax);
      } else {
        ymed = (ymin+ymax)>>1;
        tessellate (f,xmin,ymin,xmax,ymed);
        tessellate (f,xmin,ymed,xmax,ymax);
      }
    }
  }

#ifdef LIBVB_FLTK
  void Image::draw() {
    if (stage == NULL) {
      if (depth<8) stage = (unsigned char *) malloc (width*height*sizeof(unsigned char));
      else stage = (unsigned char *) pic;
    }

    if (depth<8) {
      char D = 255 / ((1<<depth)-1);
      for (int i=0; i<width*height; ++i) stage[i] = D * pic[i];
    }
    drawimage (stage,fltk::MONO,fltk::Rectangle(0,0,width,height),width);
  }

#endif

  void Image::output_png (std::string s) {
    FILE *fp = fopen (s.c_str(),"wb");

    png_structp png_ptr = png_create_write_struct
      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io (png_ptr, fp);

    png_set_IHDR (png_ptr, info_ptr, w(), h(), 8, PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);

    png_text img_info[1];
    img_info[0].compression = PNG_TEXT_COMPRESSION_NONE;
    img_info[0].key = "Title";
    img_info[0].text = (char*) label();
    img_info[0].text_length = strlen(label());

    png_set_text (png_ptr, info_ptr, img_info, 1);

    uchar *p;
    if (depth == 8) p = (uchar*) pic;
    else {
      p = (uchar*) malloc (width*height*sizeof(uchar));
      char D = 255 / ((1<<depth)-1);
      for (int i=0; i<width*height; ++i) stage[i] = D * pic[i];
    }

    for (int i=0; i<w()*h(); i += 1100) std::cerr << (int) (p[i]) << std::endl;

    png_bytep *row_pointers = (png_bytep*) png_malloc (png_ptr, h()*sizeof(png_bytep));
    for (int i=0; i<h(); ++i)
      row_pointers[i] = p + i*w();
    png_set_rows (png_ptr, info_ptr, row_pointers);

    png_write_info (png_ptr, info_ptr);
    png_write_image (png_ptr, row_pointers);
    png_write_end (png_ptr, NULL);
  }
}
