#pragma once
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

		virtual	Vector<cplx>	vec     	()                       	const	=0;
		virtual	void        	readvec 	(const Vector<cplx> & xy)	     	=0;
		virtual	Vector<cplx>	vcost   	()                       	const	=0;
		virtual	Matrix<cplx>	jacvcost	()                       	const	=0;

		virtual cplx reduce (const cplx & z) const { return z; }

		T	cost 	()	const;
		T	findn	();
	};
}
