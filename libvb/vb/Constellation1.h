#pragma once
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>
#include <vb/Polynomial.h>
#include <vb/math.h>

namespace vb {
	template <typename T> class Constellation1 { public:
		using cplx = std::complex<T>;

		Constellation1 (Hypermap M, Hub H, int n=3);
		Constellation1 ();

		cplx	operator()	(cplx z)         	const;
		cplx	logder    	(cplx z, int k=0)	const;

		void   	from_points	();                        	// compute P and Q, don't touch l
		// void	linear     	(cplx u, cplx v = cplx(0));	// move the points, recompute P and Q, don't touch l
		void   	normalize  	();                        	// choose l to make ones ones
		// void	make_l_1   	();                        	// rescale to make l equal to 1
		// void	make_c_0   	();                        	// shift everybody to make sum(black)=0
		// void	make_p_1   	();                        	// try to have reasonable scaling
		// void	belyi      	();                        	// does too many things at once

		cplx	my_lsigma	(cplx z) const;
		cplx	my_zeta  	(cplx z) const;

		T           	cost    	()	const;
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;

		void	find 	();
		void	findm	();
		void	findn	();

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t)	const;
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		Image *	draw	(unsigned l) const;

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx	l = T(1), tau = cplx(0,1), q=q_(tau);
		cplx	et1,th1p0;
		int 	dx,dy,d;
	};

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
