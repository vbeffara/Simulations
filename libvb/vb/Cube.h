#pragma once /// \file
#include <vb/Image.h>

namespace vb {
	template <typename T, typename S> class Cube : public Image {
	public:
		Cube (int x, int y, int z, const std::string &name) : Image(x+z,y+z,name), sx(x), sy(y), sz(z), data(sx*sy*sz,0) {
			for (int x=sx; x<sx+sz; ++x) for (int y=sy; y<sy+sz; ++y) Image::put (coo(x,y), ((x+y)%2) ? 255 : 0);
		}

		void update () {
			for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) {
				S s=0; for (int z=0; z<sz; ++z) s += at(x,y,z);
				Image::put (coo(x,y), s/sz);
			}
			for (int x=0; x<sx; ++x) for (int z=0; z<sz; ++z) {
				S s=0; for (int y=0; y<sy; ++y) s += at(x,y,z);
				Image::put (coo(x,z+sy), s/sz);
			}
			for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) {
				S s=0; for (int x=0; x<sx; ++x) s += at(x,y,z);
				Image::put (coo(z+sx,y), s/sz);
			}
			Image::update();
		}

		T & 	at 	(int x, int y, int z)            	{ return data[x+sx*y+sx*sy*z]; }
		T & 	atp	(int x, int y, int z)            	{ int xx=((x%sx)+sx)%sx, yy=((y%sy)+sy)%sy, zz=((z%sz)+sz)%sz; return at(xx,yy,zz); }
		void	put	(int x, int y, int z, const T &t)	{ data[x+sx*y+sx*sy*z] = t; step(); }

		int sx,sy,sz;
		std::vector<T> data;
	};
}
