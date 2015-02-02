#pragma once
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		Constellation (Hypermap M, Hub H, int n=3);
		Constellation ();

		cplx	operator()	(cplx z)         	const { return l*P(z)/Q(z); }
		cplx	logder    	(cplx z, int k=0)	const;

		void	from_points	();                        	// compute P and Q, don't touch l
		void	linear     	(cplx u, cplx v = cplx(0));	// move the points, recompute P and Q, don't touch l
		void	normalize  	();                        	// choose l to make ones ones
		void	make_l_1   	();                        	// rescale to make l equal to 1
		void	make_c_0   	();                        	// shift everybody to make sum(black)=0
		void	make_p_1   	();                        	// try to have reasonable scaling
		void	belyi      	();                        	// does too many things at once

		T           	cost    	()	const;
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;

		void	find 	(T t = T(0));
		void	findm	();
		T   	findn	();

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f)	const;
		Vector<T>   	coovec 	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f)	const;
		void        	readcoo	(const Vector<T> & xy);
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		void	draw	(Image & img, bool smooth = true) const;

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx            	l = T(1);
		Polynomial<cplx>	P,Q;
	};

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C);
}
