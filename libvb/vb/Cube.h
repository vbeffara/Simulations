#pragma once /// \file
#include <vb/Image.h>

namespace vb {
	template <typename T, typename S> class Adder {
	public:
		Adder () : s(0), n(1) { };
		void dim (int _n) { n = _n; }
		S operator+= (const T &t) { s += t; return s; }
		S operator-= (const T &t) { s -= t; return s; }
		operator Color() { return Color(s/n); }

		S s; int n;
	};

	template <typename T, typename S> class Cube : public Image {
	public:
		Cube (int x, int y, int z, const std::string &name) :Image(x+z,y+z,name),
			sx(x), sy(y), sz(z), data(sx*sy*sz,0), XY(sx*sy), YZ(sy*sz), XZ(sx*sz) {
			for (int x=sx; x<sx+sz; ++x) for (int y=sy; y<sy+sz; ++y) Image::put (coo(x,y), ((x+y)%2) ? 255 : 0);
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) XY[x+sx*y].dim(sz);
			for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z) XZ[x+sx*z].dim(sy);
			for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) YZ[y+sy*z].dim(sx);
		}

		void update () {
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y)	Image::put (coo(x,y),   	XY[x+sx*y]);
			for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z)	Image::put (coo(x,z+sy),	XZ[x+sx*z]);
			for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z)	Image::put (coo(z+sx,y),	YZ[y+sy*z]);
			Image::update();
		}

		T & 	at 	(int x, int y, int z)            	{ return data[x+sx*y+sx*sy*z]; }
		T & 	atp	(int x, int y, int z)            	{ int xx=((x%sx)+sx)%sx, yy=((y%sy)+sy)%sy, zz=((z%sz)+sz)%sz; return at(xx,yy,zz); }
		void	put	(int x, int y, int z, const T &t)	{
		    T d = data[x+sx*y+sx*sy*z]; XY[x+sx*y] -= d; XZ[x+sx*z] -= d; YZ[y+sy*z] -= d;
		    data[x+sx*y+sx*sy*z] = t; XY[x+sx*y] += t; XZ[x+sx*z] += t; YZ[y+sy*z] += t;
		    step();
		}

		int sx,sy,sz;
		std::vector<T> data;
		std::vector< Adder<T,S> > XY,YZ,XZ;
	};
}
