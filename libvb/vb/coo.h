#pragma once /// @file

#include <vb/cpx.h>

namespace vb {
	class coo { public: long x,y;
		coo(long xx = 0, long yy = 0) : x(xx), y(yy) {}

		bool	operator==	(const coo & z)	const	{ return (x == z.x) && (y == z.y); }
		bool	operator!=	(const coo & z)	const	{ return (x != z.x) || (y != z.y); }
		coo 	operator+ 	(const coo & z)	const	{ return coo (x+z.x, y+z.y); }
		coo 	operator- 	(const coo & z)	const	{ return coo (x-z.x, y-z.y); }
		coo 	operator- 	()             	const	{ return coo (-x,-y); }

		coo	operator*	(long d)	const	{ return coo (x*d,y*d); }
		coo	operator/	(long d)	const	{ return coo (x/d,y/d); }

		void	operator+=	(const coo & z)		{ x+=z.x; y+=z.y; }
		void	operator-=	(const coo & z)		{ x-=z.x; y-=z.y; }

		operator cpx() const { return cpx(x,y); }
	};

	inline long norm	(coo z) { return z.x*z.x + z.y*z.y; }
	inline long sup 	(coo z) { return std::max (abs(z.x), abs(z.y)); }

	inline std::ostream & operator<< (std::ostream & os, const coo z) { return os << "(" << z.x << "," << z.y << ")"; }

	const coo J (0,1);

	class rect { public: coo ul,br;
		rect (coo u, coo b) : ul(u), br(b) {};
	};

	class coo3 { public: long x,y,z;
		coo3 (long xx, long yy, long zz) : x(xx), y(yy), z(zz) {}

		bool	operator==	(const coo3 & c)	const	{ return (x==c.x) && (y==c.y) && (z==c.z); }
		bool	operator!=	(const coo3 & c)	const	{ return (x!=c.x) || (y!=c.y) || (z!=c.z); }
		coo3	operator+ 	(const coo3 & c)	const	{ return coo3 (x+c.x, y+c.y, z+c.z); }
	};

	extern const coo dz[8];
	extern const coo3 dz3[6];
};
