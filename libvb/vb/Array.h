#pragma once /// \file

#include <vb/PRNG.h>
#include <vb/Stream.h>
#include <vb/coo.h>
#include <vb/math.h>

namespace vb {
	template <typename T> class Array_iterator;

	template <typename T> class Array { public:
		Array (long w, long h, T d) :  ww(w), hh(h), data (w*h,d) {};
		Array (long w=0, long h=0) :  ww(w), hh(h), data (w*h) {};
		Array (const std::vector<std::vector<T>> & l) : ww(l.size()), hh(l[0].size()), data(ww*hh) {
			for (long i=0; i<ww; ++i) for (long j=0; j<hh; ++j) put(coo(i,j),l[i][j]);
		}

		void resize (long w, long h) { ww=w; hh=h; data.resize(ww*hh); }
		void resize (long w, long h, T t) { ww=w; hh=h; data.resize(ww*hh,t); }

		T &      	at	(const coo & z)      	{ return data[z.x + ww*z.y]; }
		T const &	at	(const coo & z) const	{ return data[z.x + ww*z.y]; }

		T &      	operator[]	(const coo & z)      	{ return at(z); }
		T const &	operator[]	(const coo & z) const	{ return at(z); }

		T &      	atp	(const coo & z)      	{ long x = pmod (z.x,ww), y = pmod (z.y,hh); return at(coo(x,y)); }
		T const &	atp	(const coo & z) const	{ long x = pmod (z.x,ww), y = pmod (z.y,hh); return at(coo(x,y)); }

		void	put 	(const coo & z, T const & c)	{ at(z) = c; }
		void	putp	(const coo & z, T const & c)	{ atp(z) = c; }

		bool	contains	(const coo & z, long b=0)	const	{ return (z.x>=b) && (z.y>=b) && (z.x<ww-b) && (z.y<hh-b); }
		coo 	rand    	(long b=0)               	const	{ return coo (b+prng.uniform_int(ww-2*b), b+prng.uniform_int(hh-2*b)); }

		using iterator = Array_iterator<T>;
		iterator	begin	()	{ return iterator (*this, coo(0,0)); 	}
		iterator	end  	()	{ return iterator (*this, coo(0,hh));	}

		long ww,hh;

	private:
		std::vector<T> data;
	};

	template <typename T> class Array_iterator : public coo, std::iterator<std::forward_iterator_tag,T> { public: Array<T> &b;
		Array_iterator 	(Array<T> &bb, coo z) : coo(z), b(bb)	{}
		bool operator!=	(Array_iterator<T> &o)               	{ return (&b != &o.b) || coo::operator!= (o);	}
		bool operator!=	(coo o)                              	{ return coo::operator!= (o);                	}
		void operator++	()                                   	{ x++; if (x == b.ww) { x=0; y++; }          	}
		T & operator*  	()                                   	{ return b.at(*((coo*)this));                	}
	};

	template <typename T> Stream<coo> coos (const Array<T> & A) { return Stream<coo> ([&A](Sink<coo> & yield) {
		for (long y=0; y<A.hh; ++y) for (long x=0; x<A.ww; ++x) yield(coo(x,y));
	});}
}
