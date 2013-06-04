/// @file
/// Utility include file importing external resources (plus vb/config.h)
/// needed by all the others. Exactly which ones is hidden from the
/// documentation, but you can have a look at the source. You should never
/// have to include this file directly.

#pragma once
#ifndef DOXYGEN
#include <vb/config.h>

#include <algorithm>
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

#include <cairo.h>
#include <cairo-pdf.h>

#define BOOST_DISABLE_ASSERTS 1
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/random.hpp>
#include <boost/utility.hpp>

#include <FL/Fl.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/fl_draw.H>
#endif

namespace vb {
	/// Utility type for a complex number.
	typedef std::complex<double> cpx;

	const cpx I(0,1);

	class tri { public: double x,y,z; tri (double xx, double yy, double zz) : x(xx), y(yy), z(zz) {} };
	inline std::ostream & operator<< (std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

	/// Utility type for 2d integer coordinates.
	typedef std::complex<int> coo;

	/// Utility type for 3d integer coordinates.
	class coo3 { public: int x,y,z; coo3 (int xx, int yy, int zz) : x(xx), y(yy), z(zz) {} };

	inline coo3 operator+ (const coo3 & c1, const coo3 & c2) { return coo3 (c1.x+c2.x, c1.y+c2.y, c1.z+c2.z); }

	class fmt : public boost::format {
	public:
		fmt (const std::string &s) : boost::format(s) { }
		operator std::string() const { return boost::format::str(); }
	};

	#define foreach BOOST_FOREACH

	// Convenient for steps. In order: Z^2, then NE/SW for triangular,
	// then NW/SE to finish the degree 8 lattice.
	const int dx[8] = {1,0,-1,0,1,-1,-1,1};
	const int dy[8] = {0,1,0,-1,1,-1,1,-1};
	const coo dz[8] = {coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1)};

	const coo3 dz3[6] = { coo3(1,0,0), coo3(-1,0,0), coo3(0,1,0), coo3(0,-1,0), coo3(0,0,1), coo3(0,0,-1) };
};
