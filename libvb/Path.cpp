/// @file Path.cpp
/// Implementation of the vb::Path class.

#include <vb/Path.h>
#include <vb/TriMatrix.h>

static const char dirs [4] = {'E','N','W','S'};
static const int  dx   [4] = { 1,  0, -1,  0 };
static const int  dy   [4] = { 0,  1,  0, -1 };

namespace vb {
  Path::Path (int l, const std::string & title_) : std::vector<char> (l), title(title_) { };

  bool Path::self_avoiding () {
    vb::TriMatrix<char> T;
    int l=0, x=0, y=0;

    T.put(x,y,1);
    for (int i=0; i<size(); ++i) {
      l  = (l+at(i)) % 4; x += dx[l]; y += dy[l];
      if (T.get(x,y)) return false; else T.put(x,y,1);
    }
    return true;
  }

  void Path::printout () const {
    int imin=0, imax=0, jmin=0, jmax=0, x=0, y=0;

    for (unsigned i=0; i<size(); i++) {
      x += dx[(int) at(i)];
      y += dy[(int) at(i)];
      if (x<imin) imin=x;
      if (x>imax) imax=x;
      if (y<jmin) jmin=y;
      if (y>jmax) jmax=y;
    }

    std::cout << "%!PS-Adobe-2.0 EPSF-2.0\n"
      << "%%Title: " << title << "\n"
      << "%%Creator: libvb - © 2001-2008 Vincent Beffara - GPL v3\n"
      << "%%Creator:   Mail: Vincent.Beffara@ens-lyon.fr\n"
      << "%%Creator:   Web:  <http://www.umpa.ens-lyon.fr/~vbeffara/>\n"
      << "%%BoundingBox: 0 0 " << 3*(imax-imin)+6 << " " << 3*(jmax-jmin)+6 << "\n"
      << "\n"
      << "save 20 dict begin\n"
      << "/E {3 0 rlineto} bind def /W {-3 0 rlineto} bind def\n"
      << "/N {0 3 rlineto} bind def /S {0 -3 rlineto} bind def\n"
      << "newpath " << 3-3*imin << " " << 3-3*jmin << " moveto\n";

    for (unsigned i=0; i<size(); ++i)
      std::cout << dirs[(int) at(i)] << (i%40==39 ? "\n" : " ");

    if (size()%40) std::cout << "\n";
    std::cout << "stroke end restore\n";
  }
}
