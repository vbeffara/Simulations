
/*
 * What needs to be removed eventually
 */

#include <vb/config.h>

#include <iostream>
#include <string>

namespace vb {

  int printout_path (char *p, long l, std::string title)
  {
    const char *dirs = "ENWS";
    const long dx[4] = {1,0,-1,0};
    const long dy[4] = {0,1,0,-1};

    long i, imin,imax,  jmin,jmax, x,y;

    /* Step 1 = cropping */

    imin=0; imax=0; jmin=0; jmax=0; x=0; y=0;
    for (i=0;i<l;i++) {
      x+=dx[(int)p[(int)i]];
      y+=dy[(int)p[(int)i]];
      if (x<imin) imin=x;
      if (x>imax) imax=x;
      if (y<jmin) jmin=y;
      if (y>jmax) jmax=y;
    }

    /* Step 2 = printing */

    /* Header */

    std::cout << "%!PS-Adobe-2.0 EPSF-2.0\n";
    std::cout << "%%Title: " << title << "\n";
    std::cout << "%%Creator: libvb - v" << LIBVB_VERSION << " - © 2001-2006 VB - GPL\n";
    std::cout << "%%Creator:   Mail: Vincent.Beffara@ens-lyon.fr\n";
    std::cout << "%%Creator:   Web:  <http://www.umpa.ens-lyon.fr/~vbeffara/>\n";
    std::cout << "%%BoundingBox: 0 0 " 
      << 3*(imax-imin)+6 << " " << 3*(jmax-jmin)+6 << "\n\n";

    /* "Code" ;-) */

    std::cout << "save 20 dict begin\n";
    std::cout << "/E {3 0 rlineto} bind def /W {-3 0 rlineto} bind def\n";
    std::cout << "/N {0 3 rlineto} bind def /S {0 -3 rlineto} bind def\n";
    std::cout << "newpath " << 3-3*imin << " " << 3-3*jmin << " moveto\n";

    for (i=0;i<l;) {
      std::cout << dirs[(int)p[(int)i]];
      if (!(++i%40)) std::cout << "\n";
      else std::cout << " ";
    }
    if (i%40) std::cout << "\n";

    std::cout << "stroke end restore\n";
    return 0;
  }

}
