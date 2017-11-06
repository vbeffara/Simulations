#pragma once /// @file
#include <vb/cpx.h>
#include <vector>

namespace vb {
	class coo { public: int64_t x,y;
		coo (int64_t xx, int64_t yy) : x(xx), y(yy) {}
		coo () : coo (0,0) {}

		bool	operator==	(const coo & z)	const	{ return (x == z.x) && (y == z.y); }
		bool	operator!=	(const coo & z)	const	{ return (x != z.x) || (y != z.y); }
		coo 	operator+ 	(const coo & z)	const	{ return coo (x+z.x, y+z.y); }
		coo 	operator- 	(const coo & z)	const	{ return coo (x-z.x, y-z.y); }
		coo 	operator- 	()             	const	{ return coo (-x,-y); }

		coo	operator*	(int64_t d)	const	{ return coo (x*d,y*d); }
		coo	operator/	(int64_t d)	const	{ return coo (x/d,y/d); }

		void	operator+=	(const coo & z)		{ x+=z.x; y+=z.y; }
		void	operator-=	(const coo & z)		{ x-=z.x; y-=z.y; }

		explicit operator cpx() const { return cpx(x,y); }
	};

	inline int64_t norm	(coo z) { return z.x*z.x + z.y*z.y; }
	inline int64_t sup 	(coo z) { return std::max (std::abs(z.x), std::abs(z.y)); }

	inline std::ostream & operator<< (std::ostream & os, const coo z) { return os << "(" << z.x << "," << z.y << ")"; }

	class rect { public: coo ul,br;
		rect (coo u, coo b) : ul(u), br(b) {};
	};

	class coo3 { public: int64_t x,y,z;
		coo3 (int64_t xx, int64_t yy, int64_t zz) : x(xx), y(yy), z(zz) {}

		bool	operator==	(const coo3 & c)	const	{ return (x==c.x) && (y==c.y) && (z==c.z); }
		bool	operator!=	(const coo3 & c)	const	{ return (x!=c.x) || (y!=c.y) || (z!=c.z); }
		coo3	operator+ 	(const coo3 & c)	const	{ return coo3 (x+c.x, y+c.y, z+c.z); }
	};

	extern const std::vector<coo> dz;
	extern const std::vector<coo3> dz3;

	#ifdef UNIT_TESTS
	TEST_CASE ("vb::coo") {
		coo z1 {2,3}, z2 {4,-1};
		CHECK (z1+z2 == coo(6,2));
		CHECK (z1-z2 == coo(-2,4));
		CHECK (-z1 == coo(-2,-3));
		CHECK (z1*3 == coo(6,9));
		CHECK (z1/2 == coo(1,1));

		z1 += z2; z2 -= z1;
		CHECK (cpx(z2) == cpx(-2,-3));
		CHECK (norm(z2) == 13);
		CHECK (sup(z2) == 3);
		CHECK (str(z1) == "(6,2)");
	}
	#endif
} // namespace vb
