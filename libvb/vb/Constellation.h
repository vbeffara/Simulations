#pragma once
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> struct Star {
		std::complex<T> z;
		unsigned long d;
	};

	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		std::vector<Star<T>>	b,w,f;
		std::vector<cplx>   	p;

		Constellation                      	();
		template <typename U> Constellation	(const Constellation<U> & C);

		virtual	Vector<cplx>	vec     	()                       	const	=0;
		virtual	void        	readvec 	(const Vector<cplx> & xy)	     	=0;
		virtual	Vector<cplx>	vcost   	()                       	const	=0;
		virtual	Matrix<cplx>	jacvcost	()                       	const	=0;

		virtual	cplx                	operator()	(cplx z)        	const	=0;
		virtual	cplx                	reduce    	(const cplx & z)	const	{ return z; }
		virtual	std::pair<cplx,cplx>	bounds    	()              	const	=0;

		void	draw	(Image & img, bool smooth = true, bool wow = false) const;

		T	cost 	()	const;
		T	findn	();
	};
}
