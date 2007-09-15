/// @file vb.h
/// Include a standard subset of the libvb library.

#include <vb/types.h>

#include <vb/Image.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

#include <string>

/** The namespace where libvb puts everything it defines. */

namespace vb {

  /** Makes an EPS file out of a path in Z^2.
   *
   * The path is stored as an array of chars, taking values between 0
   * and 3. The path is clipped automatically.
   *
   * @param p The path data.
   * @param l The length of the path.
   * @param title The title of the created image.
   */

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
    std::cout << "%%Creator: libvb - © 2001-2007 VB - GPL\n";
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

/** @mainpage Documentation for the `libvb' library.
 *
 * @section sec_intro Introduction
 *
 * The aim of this library is to provide an easy way for mathematicians
 * (and maybe others) to display 2D graphics for their simulations and
 * to produce EPS files for easy inclusion in a LaTeX document. The main
 * design goal is that usage should be as simple as possible; as such,
 * the library is not very efficient (no fancy OpenGL or even hardware
 * acceleration) but the interface is straightforward. Among the useful
 * features:
 *
 * - vb::Image: the main class provided by the library. It centralizes
 *   most of the functionality, can be displayed on the screen by
 *   calling vb::Image::onscreen() and transformed into an EPS file via
 *   an output stream (std::cout << img).
 *
 * - Auto-update: the display routine (namely vb::Image::putpoint())
 *   will try to update the screen representation of the image on the
 *   screen about 25 times per second. It works best if the computation
 *   time is roughly the same for all the points displayed. This is
 *   extremely practical.
 *
 * - vb::Image::tessellate(): a quick way to fill an image according to
 *   a coloring function (which gives the gray level as a function of
 *   coordinates) by a dyadic decomposition.
 *
 * - A helper class vb::CL_Parser to access a simple but often
 *   sufficient subset of getopt. Supports default values for parameters
 *   and a personalized help message. I use it myself even for programs
 *   without a display.
 *
 * - vb::CoarseImage: a simple way to store and display very large
 *   images that consist of large black and white zones (typically
 *   first-passage percolation clusters).
 *
 * - vb::PRNG: a pseudo-random number generator (and yet another
 *   implementation of the "Mersenne twister algorithm"), very fast
 *   and much better than the standard rand(). Implements various
 *   standard distributions.
 *
 * - vb::Figure: a class for building figures from a program, and 
 *   exporting it as an Asymptote file. Right now it is embryonic (only 
 *   dots, line segments and circles) but already functional.
 *
 * And once again: this is all supposed to be very easy to use:
 * everything resides in header files for easy inclusion. If it
 * isn't, please let me know (vbeffara@ens-lyon.fr) ! See the sample
 * code included in the documentation for an easy way to get started.
 *
 * @section sec_install Installation
 *
 * There is no separate package for the library (though I might produce
 * one at some point), so you will have to download all my simulations.
 * On the other hand, it will give you some sample code, which cannot
 * hurt. See http://www.umpa.ens-lyon.fr/~vbeffara/simu.php for details.
 *
 * The main dependency is on the SDL library (http://www.libsdl.org) -
 * together with SCons as a build framework, though it is very easy to
 * compile without it.
 */
