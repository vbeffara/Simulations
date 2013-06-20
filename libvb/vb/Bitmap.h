#pragma once /// \file
#include <vb/Picture.h>
#include <vb/coo.h>

namespace vb {
	template <typename T> class Bitmap_iterator;

	template <typename T> class Bitmap : public Picture { public:
		Bitmap (int wd, int ht, const std::string &tit, T d=0);
		~Bitmap () { if ((T*)stage != data) free(data); }

		T &      	at  	(coo z)             	{ return data[real(z) + stride*imag(z)]; }
		T const &	at  	(coo z) const       	{ return data[real(z) + stride*imag(z)]; }
		T &      	atp 	(coo z)             	{ int x=(real(z)%w() + w())%w(), y=(imag(z)%h() + h())%h(); return at(coo(x,y)); }
		T const &	atp 	(coo z) const       	{ int x=(real(z)%w() + w())%w(), y=(imag(z)%h() + h())%h(); return at(coo(x,y)); }
		void     	put 	(coo z, T const & c)	{ at(z) = c;  step(); }
		void     	putp	(coo z, T const & c)	{ atp(z) = c; step(); }

		bool     	contains	(coo z) { return (real(z)>=0) && (imag(z)>=0) && (real(z)<w()) && (imag(z)<h()); }
		virtual T	compute 	(coo z) { return dflt; }
		T        	lazy    	(coo z) { if (at(z) == dflt) put(z, compute(z)); return at(z); }

		void fill (coo z, T c, int adj = 4);
		void tessel (int xmin, int ymin, int xmax, int ymax);

		typedef Bitmap_iterator<T> iterator;
		iterator	begin	()	{ return iterator (*this, coo(0,0));  	}
		iterator	end  	()	{ return iterator (*this, coo(0,h()));	}

	private:
		Color * stage;	///< The raw pixel data of the screen representation.
		T * data;     	///< The actual data.
		coo z0;       	///< The coordinates of the origin (at(0) is there on screen).
		T dflt;       	///< The default value.

		virtual void paint () { if ((T*)stage == data) return; // Bitmap<Color>
			for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) stage[x+stride*y] = data[x+stride*y];
		}
	};

	template<typename T> class Bitmap_iterator : public coo { public: Bitmap<T> &b;
		Bitmap_iterator	(Bitmap<T> &bb, coo z) : coo(z), b(bb)	{}
		bool operator!=	(Bitmap_iterator<T> &o)               	{ return (&b != &o.b) || ((coo)(*this) != (coo)(o));	}
		bool operator!=	(coo o)                               	{ return ((coo)(*this) != (coo)(o));                	}
		void operator++	()                                    	{ x++; if (x == b.w()) { x=0; y++; }                	}
		T & operator*  	()                                    	{ return b.at(*this);                               	}
	};

	template<typename T> Bitmap<T>::Bitmap (int wd, int ht, const std::string &tit, T d) :
		Picture(wd,ht,tit), stage ((Color *) (cairo_image_surface_get_data (surface))),
		data((T*) calloc (stride*ht,sizeof(T))), z0(0), dflt(d) {
			for (int i=0; i<stride*ht; ++i) data[i]=dflt;
	}

	template<typename T> void Bitmap<T>::fill (coo z, T c, int adj) {
		T in = at(z); if (in == c) return;
	    std::vector<coo> xy; xy.push_back(z); at(z) = c;

		while (xy.size()) {
			coo ij = xy.back(); xy.pop_back(); for (int d=0; d<adj; ++d) {
				coo nij = ij + dz[d]; if (contains(nij) && (at(nij) == in)) { xy.push_back(nij); at(nij)=c; }
			}
		}
	}

	template<typename T> void Bitmap<T>::tessel (int xmin, int ymin, int xmax, int ymax) {
		Color tmp = lazy (coo(xmin,ymin)); bool mono = true;
		for (int i=xmin; i<=xmax; ++i) { if (lazy(coo(i,ymin)) != tmp) mono=false; if (lazy(coo(i,ymax)) != tmp) mono=false; }
		for (int j=ymin; j<=ymax; ++j) { if (lazy(coo(xmin,j)) != tmp) mono=false; if (lazy(coo(xmax,j)) != tmp) mono=false; }
		if (mono) { for (int i=xmin+1; i<xmax; ++i) for (int j=ymin+1; j<ymax; ++j) at(coo(i,j)) = tmp; }
		else if ((xmax-xmin) > std::max(ymax-ymin,1))	{ int xmed=(xmin+xmax)>>1; tessel (xmin,ymin,xmed,ymax); tessel (xmed,ymin,xmax,ymax); }
		else if (ymax>ymin+1)                        	{ int ymed=(ymin+ymax)>>1; tessel (xmin,ymin,xmax,ymed); tessel (xmin,ymed,xmax,ymax); }
	}

	template<> Bitmap<Color>::Bitmap (int wd, int ht, const std::string &tit, Color d);
	extern template class Bitmap<Color>;
}
