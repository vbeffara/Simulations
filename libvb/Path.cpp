/// @file Path.cpp
/// Implementation of the vb::Path class.

#include <vb/Path.h>

static const char *dirs = "ENWS";
static const int dx[4] = {1,0,-1,0};  /*  0=est, 1=nord ... */
static const int dy[4] = {0,1,0,-1};

namespace vb {
  Path::Path (int l, const std::string & title_) :
    std::vector<char> (l), title(title_) { };

  bool Path::self_avoiding () {
    int n=size();
    char * w = &at(0);

    std::vector<int> x_hash(2*n);
    std::vector<int> y_hash(2*n);

    int x,y,l,i,k;
    bool ok;

    x=n+1; y=n+(n<<1)+1; l=0; ok=1;
    k=(x*y)%(2*n);
    x_hash[k]=x;
    y_hash[k]=y;

    for (i=0;(i<n)&&(ok==1);i++) {
      l=(l+w[i])%4;
      x+=dx[l];
      y+=dy[l];
      k=(x*y)%(2*n);

      while ((ok)&&(x_hash[k]!=0)&&(y_hash[k]!=0)) {
        if ((x_hash[k]==x)&&(y_hash[k]==y)) ok=0;
        else k=(k+1)%(2*n);
      }

      x_hash[k]=x;
      y_hash[k]=y;
    }

    return ok;
  }

  void Path::printout () const {
    char * p = (char*) &at(0);

    long imin,imax,  jmin,jmax, x,y;
    unsigned i;

    imin=0; imax=0; jmin=0; jmax=0; x=0; y=0;
    for (i=0;i<size();i++) {
      x+=dx[(int)p[(int)i]];
      y+=dy[(int)p[(int)i]];
      if (x<imin) imin=x;
      if (x>imax) imax=x;
      if (y<jmin) jmin=y;
      if (y>jmax) jmax=y;
    }

    std::cout << "%!PS-Adobe-2.0 EPSF-2.0\n";
    std::cout << "%%Title: " << title << "\n";
    std::cout << "%%Creator: libvb - © 2001-2008 Vincent Beffara - GPL v3\n";
    std::cout << "%%Creator:   Mail: Vincent.Beffara@ens-lyon.fr\n";
    std::cout << "%%Creator:   Web:  <http://www.umpa.ens-lyon.fr/~vbeffara/>\n";
    std::cout << "%%BoundingBox: 0 0 " 
      << 3*(imax-imin)+6 << " " << 3*(jmax-jmin)+6 << "\n\n";

    std::cout << "save 20 dict begin\n";
    std::cout << "/E {3 0 rlineto} bind def /W {-3 0 rlineto} bind def\n";
    std::cout << "/N {0 3 rlineto} bind def /S {0 -3 rlineto} bind def\n";
    std::cout << "newpath " << 3-3*imin << " " << 3-3*jmin << " moveto\n";

    for (i=0;i<size();) {
      std::cout << dirs[(int)p[(int)i]];
      if (!(++i%40)) std::cout << "\n";
      else std::cout << " ";
    }
    if (i%40) std::cout << "\n";

    std::cout << "stroke end restore\n";
  }
}
