#pragma once /// \file
#include <vb/PRNG.h>
#include <vb/coo.h>
#include <vb/math.h>

namespace vb {
	template <typename T> class Array_iterator;

	template <typename T> class Array { public:
		Array (int64_t w, int64_t h, T d) :  ww(w), hh(h), data (w*h,d) {};
		Array (int64_t w, int64_t h) :  ww(w), hh(h), data (w*h) {};
		explicit Array (const std::vector<std::vector<T>> & l) : ww(l.size()), hh(l[0].size()), data(ww*hh) {
			for (int64_t i=0; i<ww; ++i) for (int64_t j=0; j<hh; ++j) put(coo(i,j),l[i][j]);
		}
		Array () : Array (0,0) {}

		void resize (int64_t w, int64_t h) { ww=w; hh=h; data.resize(ww*hh); }
		void resize (int64_t w, int64_t h, T t) { ww=w; hh=h; data.resize(ww*hh,t); }

		T &      	at	(const coo & z)      	{ return data[z.x + ww*z.y]; }
		T const &	at	(const coo & z) const	{ return data[z.x + ww*z.y]; }

		T &      	operator[]	(const coo & z)      	{ return at(z); }
		T const &	operator[]	(const coo & z) const	{ return at(z); }

		T &      	atp	(const coo & z)      	{ int64_t x = pmod (z.x,ww), y = pmod (z.y,hh); return at(coo(x,y)); }
		T const &	atp	(const coo & z) const	{ int64_t x = pmod (z.x,ww), y = pmod (z.y,hh); return at(coo(x,y)); }

		void	put 	(const coo & z, T const & c)	{ at(z) = c; }
		void	putp	(const coo & z, T const & c)	{ atp(z) = c; }

		bool	contains	(const coo & z, int64_t b=0)	const	{ return (z.x>=b) && (z.y>=b) && (z.x<ww-b) && (z.y<hh-b); }
		coo 	rand    	(int64_t b=0)               	const	{ return coo (b+prng.uniform_int(ww-2*b), b+prng.uniform_int(hh-2*b)); }

		using iterator = Array_iterator<T>;
		iterator	begin	()	{ return iterator (*this, coo(0,0)); 	}
		iterator	end  	()	{ return iterator (*this, coo(0,hh));	}

		int64_t ww,hh;

	private:
		std::vector<T> data;
	};

	template <typename T> class Array_iterator : public coo, std::iterator<std::forward_iterator_tag,T> { public: Array<T> &b;
		Array_iterator 	(Array<T> &bb, coo z) : coo(z), b(bb)	{}
		bool operator!=	(const Array_iterator<T> &o)         	{ const coo &c = *this, &oo=o; return (&b != &o.b) || c != oo;	}
		bool operator!=	(coo o)                              	{ const coo &c = *this; return c != (o);                      	}
		void operator++	()                                   	{ x++; if (x == b.ww) { x=0; y++; }                           	}
		T & operator*  	()                                   	{ return b.at(*((coo*)this));                                 	}
	};
} // namespace vb
