#pragma once
#include <vb/Elliptic.h>
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Image.h>
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> class Constellation1 { public:
		using cplx = std::complex<T>;

		Constellation1 (const Hypermap & M, const Hub & H);
		Constellation1 ();

		void	from_points	();
		cplx	operator() 	(cplx z)	const;
		T   	cost       	()      	const;
		T   	findn      	();
		cplx	reduce     	(cplx z)	const;

		void	draw	(Image & img, bool smooth = true) const;

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx       	ll = T(0), tau = cplx(0,1), q = q_(tau), qt = q_t(tau);
		Elliptic<T>	E { q };

		int	dx,dy,d;

	private:
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;
		Matrix<cplx>	jacnum  	();

		cplx	logderp  	(cplx z, int k) const;
		cplx	logderp_z	(cplx z, int k) const;
		cplx	logderp_t	(cplx z, int k) const;
		cplx	logder   	(cplx z, int k)	const;
		cplx	logder_z 	(cplx z, int k)	const;
		cplx	logder_t 	(cplx z, int k)	const;

		void	normalize	();
		void	shift    	(cplx z);

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f, const cplx & t, const cplx & l)	const;
		void        	readvec	(const Vector<cplx> & xy);
	};

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
