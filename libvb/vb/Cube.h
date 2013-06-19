#pragma once /// \file
#include <vb/Bitmap.h>
#include <vb/PRNG.h>

namespace vb {
	class Adder { public: int s; int n;
		Adder         	(int _s = 0) : s(_s), n(1)	{}
		void dim      	(int _n)                  	{ n = _n; }
		int operator+=	(int t)                   	{ s += t; return s; }
		int operator-=	(int t)                   	{ s -= t; return s; }
		operator Color	()                        	{ return Color(s/n); }
	};

	class coo3 { public: int x,y,z; coo3 (int xx, int yy, int zz) : x(xx), y(yy), z(zz) {} };
	const coo3 dz3[6] { coo3(1,0,0), coo3(-1,0,0), coo3(0,1,0), coo3(0,-1,0), coo3(0,0,1), coo3(0,0,-1) };
	inline coo3 operator+ (const coo3 & c1, const coo3 & c2) { return coo3 (c1.x+c2.x, c1.y+c2.y, c1.z+c2.z); }
	inline bool operator!= (const coo3 & c1, const coo3 & c2) { return (c1.x!=c2.x) || (c1.y!=c2.y) || (c1.z!=c2.z); }

	class Cube_iterator;

	class Cube : public Bitmap <Adder> { public:
		Cube (int x, int y, int z, const std::string &name);

		int 	index	(coo3 c) 	{ return c.x + sx*c.y + sx*sy*c.z; }
		coo3	rand 	(int b=0)	{ return coo3 (b+prng.uniform_int(sx-2*b), b+prng.uniform_int(sy-2*b), b+prng.uniform_int(sz-2*b)); }
		coo3	wrap 	(coo3 c) 	{ int xx=((c.x%sx)+sx)%sx, yy=((c.y%sy)+sy)%sy, zz=((c.z%sz)+sz)%sz; return coo3(xx,yy,zz); }

		void	putp	(coo3 c, unsigned char t)	{ put(wrap(c),t); }
		bool	done	(coo3 &c)                	{ return (c.z==sz); }

		unsigned char &	at 	(coo3 c) { return data[index(c)]; }
		unsigned char &	atp	(coo3 c) { return at(wrap(c)); }

		using Bitmap <Adder> :: at;
		void put (coo3 c, unsigned char t) {
		    unsigned char d = data[index(c)]; if (t!=d) {
				at(coo(c.x,c.y)) -= d; at(coo(c.x,c.z+sy)) -= d; at(coo(c.z+sx,c.y)) -= d;
				at(coo(c.x,c.y)) += t; at(coo(c.x,c.z+sy)) += t; at(coo(c.z+sx,c.y)) += t;
			    data[index(c)] = t;
			}
		    step();
		}

		void output_pov ();

		typedef Cube_iterator iterator;
		iterator	begin	();
		iterator	end  	();

		int sx,sy,sz; std::vector <unsigned char> data;
	};

	class Cube_iterator : public coo3 { public: Cube &c;
		Cube_iterator                  	(Cube &cc, coo3 xyz) : coo3(xyz), c(cc)	{}
		bool operator!=                	(Cube_iterator &o)                     	{ return (&c != &o.c) || ((coo3)(*this) != (coo3)(o));          	}
		void operator++                	()                                     	{ x++; if (x == c.sx) { x=0; y++; } if (y == c.sy) { y=0; z++; }	}
		unsigned const char & operator*	()                                     	{ return c.at((coo3)(*this));                                   	}
	};
}
