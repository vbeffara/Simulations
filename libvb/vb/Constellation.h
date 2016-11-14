#pragma once
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> struct Star {
		typename cpx_t<T>::type z;
		unsigned long d;
	};

	template <typename T> class Constellation { public:
		using cplx = typename cpx_t<T>::type;

		std::vector<Star<T>>	b,w,f;
		std::vector<cplx>   	p;

		unsigned dim {0};

		Constellation ();
		Constellation (const Constellation &) = default;
		Constellation (Constellation &&) = default;
		Constellation & operator= (const Constellation &) = default;
		Constellation & operator= (Constellation &&) = default;

		template <typename U> Constellation	(const Constellation<U> & C);

		virtual ~ Constellation () = default;

		virtual	Vector<cplx>	vec     	()                       	const	=0;
		virtual	void        	readvec 	(const Vector<cplx> & xy)	     	=0;
		virtual	Vector<cplx>	vcost   	()                       	const	=0;
		virtual	Matrix<cplx>	jacvcost	()                       	const	=0;

		virtual	cplx                	operator()	(cplx z)	const	=0;
		virtual	cplx                	reduce    	(cplx z)	const	{ return z; }
		virtual	std::pair<cplx,cplx>	bounds    	()      	const	=0;

		T	cost 	()	const;
		T	findn	();
	};

	template<> template<> Constellation<real_t>::Constellation (const Constellation<double> & C);
}
