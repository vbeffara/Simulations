/// @file
/// Definitions for the vb::Bitmap class

#pragma once
#include <vb/Picture.h>

namespace vb {
	template <typename T> class Bitmap : public Picture {
	public:
		Bitmap (int wd, int ht, const std::string &tit, int d=0);
		~Bitmap () { if ((T*)stage != data) free(data); }

		T       & at (coo z)       { return data[real(z) + stride*imag(z)]; }
		T const & at (coo z) const { return data[real(z) + stride*imag(z)]; }

		T       & at (int x, int y)       __attribute__ ((deprecated)) { return at(coo(x,y)); }
		T const & at (int x, int y) const __attribute__ ((deprecated)) { return at(coo(x,y)); }

		void put (coo z, T const & c) { at(z) = c; step(); }

		bool contains (coo z) {
			return (real(z)>=0) && (imag(z)>=0) && (real(z)<w()) && (imag(z)<h());
		}

	private:
		Color * stage;	///< The raw pixel data of the screen representation.
		T * data;     	///< The actual data.
		coo z0;       	///< The coordinates of the origin (at(0) is there on screen).

		virtual void paint ();
	};

	template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit, int d) :
		Picture(wd,ht,tit),
		stage ((Color *) (cairo_image_surface_get_data (surface))),
		data((T*) calloc (stride*ht,sizeof(T))), z0(0) {
			for (int i=0; i<stride*ht; ++i) data[i]=d;
	}

	template<typename T> void Bitmap<T>::paint () {
		if ((T*)stage == data) return; // Bitmap<Color>
		for (int x=0; x<w(); ++x)
			for (int y=0; y<h(); ++y)
				stage[x+stride*y] = data[x+stride*y];
	}

	template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, int d);
}
