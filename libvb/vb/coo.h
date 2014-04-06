#pragma once /// @file

#include <algorithm>
#include <cstdlib>
#include <iostream>

namespace vb {
	class coo { public: int x,y;
		coo(int xx = 0, int yy = 0) : x(xx), y(yy) {}

		bool	operator==	(const coo & z)	const	{ return (x == z.x) && (y == z.y); }
		bool	operator!=	(const coo & z)	const	{ return (x != z.x) || (y != z.y); }
		coo 	operator+ 	(const coo & z)	const	{ return coo (x+z.x, y+z.y); }
		coo 	operator- 	(const coo & z)	const	{ return coo (x-z.x, y-z.y); }
		coo 	operator- 	()             	const	{ return coo (-x,-y); }

		coo	operator/	(int d)	const	{ return coo (x/d,y/d);}

		void	operator+=	(const coo & z)		{ x+=z.x; y+=z.y; }
	};

	inline int norm	(coo z) { return z.x*z.x + z.y*z.y; }
	inline int sup 	(coo z) { return std::max (abs(z.x), abs(z.y)); }

	inline std::ostream & operator<< (std::ostream & os, const coo z) { return os << "(" << z.x << "," << z.y << ")"; }

	const coo dz[8] = { coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1) };
	const coo J (0,1);

	class rect { public: coo ul,br;
		rect (coo u, coo b) : ul(u), br(b) {};
	};

	class coo3 { public: int x,y,z;
		coo3 (int xx, int yy, int zz) : x(xx), y(yy), z(zz) {}

		bool	operator==	(const coo3 & c)	const	{ return (x==c.x) && (y==c.y) && (z==c.z); }
		bool	operator!=	(const coo3 & c)	const	{ return (x!=c.x) || (y!=c.y) || (z!=c.z); }
		coo3	operator+ 	(const coo3 & c)	const	{ return coo3 (x+c.x, y+c.y, z+c.z); }
	};

	const coo3 dz3[6] = { coo3(1,0,0), coo3(-1,0,0), coo3(0,1,0), coo3(0,-1,0), coo3(0,0,1), coo3(0,0,-1) };
};
