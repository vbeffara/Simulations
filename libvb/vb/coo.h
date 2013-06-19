#pragma once /// @file

namespace vb {
	class coo { public: int x,y;
		coo(int xx = 0, int yy = 0) : x(xx), y(yy) {}
		void operator+= (coo z) { x+=z.x; y+=z.y; }
	};

	inline bool operator!= (coo z1, coo z2) { return (z1.x != z2.x) || (z1.y != z2.y); }
	inline bool operator== (coo z1, coo z2) { return (z1.x == z2.x) && (z1.y == z2.y); }

	inline coo operator+ (coo z1, coo z2) { return coo (z1.x+z2.x, z1.y+z2.y); }
	inline coo operator- (coo z1, coo z2) { return coo (z1.x-z2.x, z1.y-z2.y); }
	inline coo operator- (coo z) { return coo (-z.x,-z.y); }

	inline int real (coo z) { return z.x; }
	inline int imag (coo z) { return z.y; }
	inline int norm (coo z) { return z.x*z.x + z.y*z.y; }

	const coo dz[8] = {coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1)};
};
