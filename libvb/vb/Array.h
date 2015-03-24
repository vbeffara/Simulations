#pragma once /// \file
#include <vb/PRNG.h>
#include <vb/coo.h>

namespace vb {
	template <typename T> class Array_iterator;

	template <typename T> class Array { public:
		Array (int w, int h, T d) :  ww(w), hh(h), data (w*h,d) {};

		T &      	at	(coo z)      	{ return data[z.x + ww*z.y]; }
		T const &	at	(coo z) const	{ return data[z.x + ww*z.y]; }

		T &      	atp	(coo z)      	{ int x=(z.x%ww + ww)%ww, y=(z.y%hh + hh)%hh; return at(coo(x,y)); }
		T const &	atp	(coo z) const	{ int x=(z.x%ww + ww)%ww, y=(z.y%hh + hh)%hh; return at(coo(x,y)); }

		void	put 	(coo z, T const & c)	{ at(z) = c; }
		void	putp	(coo z, T const & c)	{ atp(z) = c; }

		bool	contains	(coo z)  	{ return (z.x>=0) && (z.y>=0) && (z.x<ww) && (z.y<hh); }
		coo 	rand    	(int b=0)	{ return coo (b+prng.uniform_int(ww-2*b), b+prng.uniform_int(hh-2*b)); }

		typedef Array_iterator<T> iterator;
		iterator	begin	()	{ return iterator (*this, coo(0,0)); 	}
		iterator	end  	()	{ return iterator (*this, coo(0,hh));	}

		int ww,hh;

	private:
		std::vector<T> data;
	};

	template<typename T> class Array_iterator : public coo { public: Array<T> &b;
		Array_iterator 	(Array<T> &bb, coo z) : coo(z), b(bb)	{}
		bool operator!=	(Array_iterator<T> &o)               	{ return (&b != &o.b) || ((coo)(*this) != (coo)(o));	}
		bool operator!=	(coo o)                              	{ return ((coo)(*this) != (coo)(o));                	}
		void operator++	()                                   	{ x++; if (x == b.ww) { x=0; y++; }                 	}
		T & operator*  	()                                   	{ return b.at(*this);                               	}
	};
}
