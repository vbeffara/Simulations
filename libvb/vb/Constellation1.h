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

		cplx	operator()	(cplx z)	const;
		cplx	logderp   	(cplx z, int k) const;
		cplx	logderp_z 	(cplx z, int k) const;
		cplx	logderp_t 	(cplx z, int k) const;
		cplx	logder    	(cplx z, int k)	const;
		cplx	logder_z  	(cplx z, int k)	const;
		cplx	logder_t  	(cplx z, int k)	const;

		void	from_points	();	// compute P and Q, don't touch l
		void	normalize  	();	// choose l to make ones ones

		cplx	my_lsigma  	(cplx z) const;
		cplx	my_lsigma_z	(cplx z) const;
		cplx	my_lsigma_t	(cplx z) const;
		cplx	my_zeta    	(cplx z) const;
		cplx	my_zeta_z  	(cplx z) const;
		cplx	my_zeta_t  	(cplx z) const;
		cplx	my_wp      	(cplx z) const;
		cplx	my_wp_z    	(cplx z) const;
		cplx	my_wp_t    	(cplx z) const;
		cplx	my_wp_zz   	(cplx z) const;
		cplx	my_wp_zt   	(cplx z) const;

		T           	cost    	()	const;
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;
		Matrix<cplx>	jacnum  	();

		T	find 	(T eps = T(1e-100));
		T	findn	();

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t, const cplx & l)	const;
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		Image *	draw	(unsigned l) const;

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx	ll = T(0), tau = cplx(0,1), q=q_(tau);
		cplx	et1,th1p0,mt3t4p2,mt3t4p2_q;
		int 	dx,dy,d;
	};

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
