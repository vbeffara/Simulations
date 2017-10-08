#pragma once /// @file
#include <iostream>
#include <vb/config.h>

namespace vb {
	class Color {
	public:
#ifdef BIGENDIAN
		unsigned char a; ///< The alpha channel (for ARGB32).
		unsigned char r; ///< The red component.
		unsigned char g; ///< The green component.
		unsigned char b; ///< The blue component.

		Color (unsigned char R, unsigned char G, unsigned char B, unsigned char A=255) 	: a(A), r(R), g(G), b(B)	{}
#else
		unsigned char b; ///< The blue component.
		unsigned char g; ///< The green component.
		unsigned char r; ///< The red component.
		unsigned char a; ///< The alpha channel (for ARGB32).

		Color (unsigned char R, unsigned char G, unsigned char B, unsigned char A=255)	: b(B), g(G), r(R), a(A) {}
#endif
		Color () : Color (0,0,0,0) {}
		explicit Color (unsigned char V) : Color (V,V,V) {}

		bool operator== (const Color &o) const { return (r==o.r) && (g==o.g) && (b==o.b) && (a==o.a); }
		bool operator!= (const Color &o) const { return (r!=o.r) || (g!=o.g) || (b!=o.b) || (a!=o.a); }

		explicit operator int () { return (r+g+b)/3; }
	};

	const Color	NOCOLOR(0,0,0,0), BLACK(0,0,0), WHITE(255,255,255), RED(255,0,0), GREEN(0,255,0), BLUE(0,0,255),
	           	CYAN(0,255,255), MAGENTA(255,0,255), YELLOW(255,255,0);

	inline Color HSV (double h, double s, double v) {
		int h_i = h*6; double f = h*6 - h_i; v*=255;
		int p = v*(1-s), q = v*(1-f*s), t = v*(1-(1-f)*s);
		if (h_i==0)     	return Color(v,t,p);
		else if (h_i==1)	return Color(q,v,p);
		else if (h_i==2)	return Color(p,v,t);
		else if (h_i==3)	return Color(p,q,v);
		else if (h_i==4)	return Color(t,p,v);
		else            	return Color(v,p,q);
	}

	inline Color Indexed (int i, double s=1, double v=1) { double x = i * 1.61803398874989484820; return HSV (x-int(x),s,v); }

	inline std::ostream & operator<< (std::ostream & o, const Color & c) {
		return o << "RGBA(" << c.r << "," << c.g << "," << c.b << "," << c.a << ")";
	}
} // namespace vb
