#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
	class Adder { public: int s; int n=1;
		explicit Adder          (int _s = 0) : s(_s)	{}
		void dim                (int _n)            	{ n = _n; }
		int operator+=          (int t)             	{ s += t; return s; }
		int operator-=          (int t)             	{ s -= t; return s; }
		explicit operator Color ()                  	{ return Color(s/n); }
	};

	class Cube_iterator;

	class Cube : public Bitmap <Adder> { public:
		Cube (int x, int y, int z);

		int 	index	(coo3 c) 	{ return c.x + sx*c.y + sx*sy*c.z; }
		coo3	rand 	(int b=0)	{ return coo3 (b+prng.uniform_int(sx-2*b), b+prng.uniform_int(sy-2*b), b+prng.uniform_int(sz-2*b)); }
		coo3	wrap 	(coo3 c) 	{ int xx=((c.x%sx)+sx)%sx, yy=((c.y%sy)+sy)%sy, zz=((c.z%sz)+sz)%sz; return coo3(xx,yy,zz); }

		void	putp	(const coo3 &c, uchar t)	{ put(wrap(c),t); }
		bool	done	(const coo3 &c)                 	{ return (c.z==sz); }

		uchar &	at 	(const coo3 &c) { return data[index(c)]; }
		uchar &	atp	(const coo3 &c) { return at(wrap(c)); }

		using iterator = Cube_iterator; iterator begin (); iterator end ();

		using Bitmap <Adder> :: at;
		void put (const coo3 &c, uchar t) {
		    uchar d = data[index(c)]; if (t!=d) {
				at(coo(c.x,c.y)) -= d; at(coo(c.x,c.z+sy)) -= d; at(coo(c.z+sx,c.y)) -= d;
				at(coo(c.x,c.y)) += t; at(coo(c.x,c.z+sy)) += t; at(coo(c.z+sx,c.y)) += t;
			    data[index(c)] = t;
			}
		    step();
		}

		void output_pov (std::string s = "");

		int sx,sy,sz; std::vector <uchar> data;
		bool mirrors = true;
	};

	class Cube_iterator : public coo3 { public: Cube *c;
		Cube_iterator           (Cube *cc, coo3 xyz) : coo3(xyz), c(cc)	{}
		bool operator!=         (const Cube_iterator &o)               	{ return (c != o.c) || coo3::operator!= (o);                  	}
		void operator++         ()                                     	{ x++; if (x == c->sx) { x=0; y++; } if (y == c->sy) { y=0; z++; }	}
		const uchar & operator*	()                                     	{ return c->at((coo3)(*this));                                  	}
	};
} // namespace vb
