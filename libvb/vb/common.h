/// @file
/// Utility include file importing external resources (plus vb/config.h) 
/// needed by all the others. Exactly which ones is hidden from the 
/// documentation, but you can have a look at the source. You should never 
/// have to include this file directly.

#ifndef __VB_COMMON_H
#define __VB_COMMON_H

#include <vb/config.h>

#include <cassert>
#include <cmath>
#include <complex>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <stdexcept>
#include <string>
#include <typeinfo>
#include <vector>

#include <sys/param.h>
#include <sys/time.h>

#include <cairomm/context.h>
#include <cairomm/surface.h>

#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/utility.hpp>

#ifdef HAVE_FLTK
#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Output.H>
#include <FL/fl_draw.H>
#endif

#ifdef __GNUC__
#define VB_DEPRECATED(func) func __attribute__ ((deprecated))
#elif defined(_MSC_VER)
#define VB_DEPRECATED(func) __declspec(deprecated) func
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define VB_DEPRECATED(func) func
#endif

namespace vb {
  /// Utility type for a complex number.
  typedef std::complex<double> cpx;

  /// Return the smaller of two real numbers.
  template <typename T> T min (T x, T y) { return (x<y?x:y); }

  /// Return the larger of two real numbers.
  template <typename T> T max (T x, T y) { return (x>y?x:y); }

  class fmt : public boost::format {
    public:
      fmt (const std::string &s) : boost::format(s) { }
      operator std::string() const { return boost::format::str(); }
  };

#define foreach BOOST_FOREACH
#define TWO_PI (6.28318530717958)

  const int dx[4] = {1,0,-1,0};
  const int dy[4] = {0,1,0,-1};
};

#endif

/** @mainpage Documentation for the `libvb' library.
 *
 * @section sec_intro Introduction
 *
 * The aim of this library is to provide an easy way for mathematicians
 * (and maybe others) to display 2D graphics for their simulations and
 * to produce PNG files for easy inclusion in a LaTeX document. The main
 * design goal is that usage should be as simple as possible; as such,
 * the library is not very efficient (no fancy OpenGL or even hardware
 * acceleration) but the interface is straightforward. Among the useful
 * features:
 *
 * - vb::Image: the main class provided by the library. It centralizes
 *   most of the functionality, can be displayed on the screen by
 *   calling vb::Image::show() and transformed into an PNG file.
 *
 * - Auto-update: the display routine (namely vb::Image::putpoint())
 *   will try to update the screen representation of the image on the
 *   screen about 25 times per second. It works best if the computation
 *   time is roughly the same for all the points displayed. This is
 *   extremely practical. This is wrapped into a class called 
 *   vb::AutoWindow so that it can be re-used (by vb::Figure for 
 *   instance).
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
 * - vb::Map: a class to encode planar maps, implementing various 
 *   algorithms as I need them (circle packing for instance).
 *
 * - vb::Minimizer: to minimize a function of potentially many 
 *   variables, using one of a few available algorithms 
 *   (Fletcher-Reeves, BFGS ... see the documentation for the whole 
 *   list).
 *
 * @section sec_install Installation
 *
 * There is no separate package for the library (though I might produce
 * one at some point), so you will have to download all my simulations.
 * On the other hand, it will give you some sample code, which cannot
 * hurt. See http://www.umpa.ens-lyon.fr/~vbeffara/simu.php for details.
 *
 * The main dependencies are on CairoMM for drawing (mandatory)
 * and the FLTK toolkit (http://www.fltk.org) for display -
 * though everything will happily build without it. If libpng
 * (http://www.libpng.org/pub/png/libpng.html) is installed, most of
 * the objects can create PNG versions of themselves, and soon movie
 * creation should be possible.
 *
 * I am using waf (http://code.google.com/p/waf/) as a build framework,
 * but if you don't have it installed, there is one included in the
 * source and a Makefile that will use it, so typing 'make' will work -
 * provided you at least have Python, which is very likely.
 *
 * Everything I wrote in the package is distributed under the terms of 
 * the GNU GPL license, version 3 (a copy of which is contained in the 
 * archive of the library). The contents of the xtoys/ directory are not 
 * written by me, and the license on it is not clear, I am only 
 * including it for convenience (so that it can benefit from the waf 
 * framework). I will remove it if asked.
 */
