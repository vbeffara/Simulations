#pragma once /// \file
#include <vb/Bitmap.h>
#include <vb/Pov.h>

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

		T &	at 	(int x, int y, int z)	{                                                             	return data[x+sx*y+sx*sy*z];	}
		T &	atp	(int x, int y, int z)	{ int xx=((x%sx)+sx)%sx, yy=((y%sy)+sy)%sy, zz=((z%sz)+sz)%sz;	return at(xx,yy,zz);        	}

		void put (int x, int y, int z, const T &t) {
		    T d = data[x+sx*y+sx*sy*z]; if (t!=d) {
				at(coo(x,y)) -= d; at(coo(x,z+sy)) -= d; at(coo(z+sx,y)) -= d;
				at(coo(x,y)) += t; at(coo(x,z+sy)) += t; at(coo(z+sx,y)) += t;
			    data[x+sx*y+sx*sy*z] = t;
			}
		    AutoWindow::step();
		}

		void output_pov () {
			Pov_Scene SS (this->title);
			SS	<< new Pov_Camera      	(tri(7*sx/4,1.3*sy,-sz), tri(sx/2,sy/2,sz/2), 60)
			  	<< new Pov_Light_Source	(tri(1.25*sx,1.8*sy,-2.0*sz))
			  	<< new Pov_Plane       	(tri(1,0,0), -sx/4, "pigment { color White } } finish { reflection {.8} ambient 0 diffuse 0")
			  	<< new Pov_Plane       	(tri(0,1,0), -sy/8, "pigment { color White } } finish { reflection {.8} ambient 0 diffuse 0")
			  	<< new Pov_Plane       	(tri(0,0,1), 5*sz/4, "pigment { color White } } finish { reflection {.8} ambient 0 diffuse 0")
			  	<< new Pov_Frame       	(tri(0,0,0), tri(sx,sy,sz), "pigment { color Red }");
			Pov_Union * SQ = new Pov_Union("pigment { color rgb <.3,.7,1> }");
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) if (at(x,y,z)==255)
				*SQ << new Pov_Box (tri(x,y,z), tri(x+1,y+1,z+1));
			SS << SQ; SS.output_pov();
		}

		int sx,sy,sz;
		std::vector<T> data;
	};
}
