#pragma once /// \file
#include <vb/Bitmap.h>
#include <vb/Pov.h>
#include <vb/PRNG.h>

namespace vb {
	template <typename T, typename S> class Adder {
	public:
		Adder (int _s = 0) : s(_s), n(1) { };
		void dim (int _n) { n = _n; }
		S operator+= (const T &t) { s += t; return s; }
		S operator-= (const T &t) { s -= t; return s; }
		operator Color() { return Color(s/n); }

		S s; int n;
	};

	template <typename T, typename S> class Cube : public Bitmap < Adder<T,S> > {
	public:
		Cube (int x, int y, int z, const std::string &name) : Bitmap < Adder<T,S> > (x+z,y+z,name), sx(x), sy(y), sz(z), data(sx*sy*sz,0) {
			for (int x=0; x<sz; ++x) for (int y=0; y<sz; ++y) at(coo(sx+x,sy+y)) = (x/10+y/10) % 2 ? 200 : 150;
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) at(coo(x,y)).dim(sz);
			for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z) at(coo(x,z+sy)).dim(sy);
			for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) at(coo(z+sx,y)).dim(sx);
		}

		using Bitmap < Adder<T,S> > :: at;

		int index (coo3 c) { return c.x + sx*c.y + sx*sy*c.z; }

		T &	at 	(coo3 c)	{                                                                   	return data[index(c)];    	}
		T &	atp	(coo3 c)	{ int xx=((c.x%sx)+sx)%sx, yy=((c.y%sy)+sy)%sy, zz=((c.z%sz)+sz)%sz;	return at(coo3(xx,yy,zz));	}

		void put (coo3 c, const T &t) {
		    T d = data[index(c)]; if (t!=d) {
				at(coo(c.x,c.y)) -= d; at(coo(c.x,c.z+sy)) -= d; at(coo(c.z+sx,c.y)) -= d;
				at(coo(c.x,c.y)) += t; at(coo(c.x,c.z+sy)) += t; at(coo(c.z+sx,c.y)) += t;
			    data[index(c)] = t;
			}
		    AutoWindow::step();
		}

		void putp (coo3 c, const T &t) { int xx=((c.x%sx)+sx)%sx, yy=((c.y%sy)+sy)%sy, zz=((c.z%sz)+sz)%sz; put(coo3(xx,yy,zz),t); }

		coo3 rand (int b=0) { return coo3 (b+prng.uniform_int(sx-2*b), b+prng.uniform_int(sy-2*b), b+prng.uniform_int(sz-2*b)); }

		void output_pov () {
			Pov_Scene SS (this->title);
			SS	<< new Pov_Camera      	(tri(1.25*sx,.9*sy,-1.5*sz), tri(0,0,0), 60)
			  	<< new Pov_Light_Source	(tri(2.5*sx,1.5*sy,-.8*sz))
			  	<< new Pov_Plane       	(tri(1,0,0), -.75*sx,	"pigment { color White } } finish { reflection {.6} ambient 0 diffuse 0")
			  	<< new Pov_Plane       	(tri(0,1,0), -.75*sy,	"pigment { color White } } finish { reflection {0} ambient 0.2 diffuse 0.3")
			  	<< new Pov_Plane       	(tri(0,0,1),  .75*sz,	"pigment { color White } } finish { reflection {.6} ambient 0 diffuse 0");

			Pov_Union * squares = new Pov_Union("pigment { color rgb <.3,.5,.8> } normal { bumps .1 scale .1 } finish { reflection {0} ambient 0 diffuse .5 brilliance 1.5 roughness .1 }");
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(coo3(x,y,z))==255)
				* squares << new Pov_Box (tri(x-.5*sx,y-.5*sy,z-.5*sz), tri(x+1-.5*sx,y+1-.5*sy,z+1-.5*sz));

			Pov_Union * cube = new Pov_Union();
			*cube	<< new Pov_Frame	(tri(-.5*sx,-.5*sy,-.5*sz), tri(.5*sx,.5*sy,.5*sz), "pigment { color rgb <.8,0,0> }")
			     	<< squares
			     	<< new Pov_Raw ("rotate 360*clock*y");

			SS << cube; SS.output_pov();
		}

		int sx,sy,sz;
		std::vector<T> data;
	};
}
