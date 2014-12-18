#pragma once /// \file
#include <vb/Picture.h>
#include <vb/Array.h>
#include <vb/cpx.h>

namespace vb {
	template <typename T> class Bitmap : public Picture, public Array<T> { public:
		Bitmap (int wd, int ht, const std::string &tit, T d=0);

		using Array<T>::at;   	using Array<T>::atp;
		using Array<T>::begin;	using Array<T>::end;
		using Array<T>::contains; // TODO: rename, clashes with Fl_Widget

		void	put 	(coo z, T const & c)	{ Array<T>::put(z,c);  step(); }
		void	putp	(coo z, T const & c)	{ Array<T>::putp(z,c); step(); }

		T	lazy	(coo z, std::function <T(coo)> f)	{ if (at(z) == dflt) put(z, f(z)); return at(z); }

		void fill (coo z, T c, int adj = 4);
		void tessel (int xmin, int ymin, int xmax, int ymax, std::function <T(coo)> f);

	private:
		Color * stage;	///< The raw pixel data of the screen representation.
		coo z0;       	///< The coordinates of the origin (at(0) is there on screen).
		T dflt;       	///< The default value.

	protected:
		virtual void paint () {
			for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) stage[x+stride*y] = at(coo(x,y));
		}
	};

	template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit, T d) :
		Picture(wd,ht,tit), Array<T>(wd,ht,d), stage ((Color *) (cairo_image_surface_get_data (surface))), z0(0), dflt(d) {}

	template<typename T> void Bitmap<T>::fill (coo z, T c, int adj) {
		T in = at(z); if (in == c) return;
	    std::vector<coo> xy; xy.push_back(z); at(z) = c;

		while (xy.size()) {
			coo ij = xy.back(); xy.pop_back(); for (int d=0; d<adj; ++d) {
				coo nij = ij + dz[d]; if (contains(nij) && (at(nij) == in)) { xy.push_back(nij); at(nij)=c; }
			}
		}
	}

	template <typename T> std::function <Color(coo)> aa (std::function <Color(std::complex<T>)> f) {
		return [&](coo c) {
			using cplx = std::complex<T>;
			cplx z (c.x,c.y); int r(0), g(0), b(0), a(0);
			for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) {
				Color c = f(z + cplx{i/3.0,j/3.0}); r+=c.r; g+=c.g; b+=c.b; a+=c.a;
			}
			return Color(r/9,g/9,b/9,a/9);
		};
	}

	template <typename T> void Bitmap<T>::tessel (int xmin, int ymin, int xmax, int ymax, std::function <T(coo)> f) {
		Color tmp = lazy (coo(xmin,ymin),f); bool mono = true;
		for (int i=xmin; i<=xmax; ++i) { if (lazy(coo(i,ymin),f) != tmp) mono=false; if (lazy(coo(i,ymax),f) != tmp) mono=false; }
		for (int j=ymin; j<=ymax; ++j) { if (lazy(coo(xmin,j),f) != tmp) mono=false; if (lazy(coo(xmax,j),f) != tmp) mono=false; }
		if (mono) { for (int i=xmin+1; i<xmax; ++i) for (int j=ymin+1; j<ymax; ++j) at(coo(i,j)) = tmp; }
		else if ((xmax-xmin) > std::max(ymax-ymin,1))	{ int xmed=(xmin+xmax)>>1; tessel (xmin,ymin,xmed,ymax,f); tessel (xmed,ymin,xmax,ymax,f); }
		else if (ymax>ymin+1)                        	{ int ymed=(ymin+ymax)>>1; tessel (xmin,ymin,xmax,ymed,f); tessel (xmin,ymed,xmax,ymax,f); }
	}
}
