#pragma once
#include <vb/Constellation.h>
#include <vb/Elliptic.h>

namespace vb {
	template <typename T> class Constellation1 : public Constellation<T> { public:
		using cplx = std::complex<T>;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f;

		Constellation1 (const Hypermap & M, const Hub & H);
		Constellation1 ();

		void	from_points	();
		cplx	operator() 	(cplx z)	const;
		T   	cost       	()      	const;
		T   	findn      	();

		void	draw	(Image & img, bool smooth = true) const;

		cplx       	ll = T(0), tau = cplx(0,1), q = q_(tau), qt = q_t(tau);
		Elliptic<T>	E { q };

		int	dx,dy,d;

	private:
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;
		Matrix<cplx>	jacnum  	();

		cplx	reduce   	(cplx z)	const;
		cplx	logderp  	(cplx z, int k) const;
		cplx	logderp_z	(cplx z, int k) const;
		cplx	logderp_t	(cplx z, int k) const;
		cplx	logder   	(cplx z, int k)	const;
		cplx	logder_z 	(cplx z, int k)	const;
		cplx	logder_t 	(cplx z, int k)	const;

		void	normalize	();
		void	shift    	(cplx z);

		Vector<cplx>	vec    	(const cplx & t, const cplx & l)	const;
		void        	readvec	(const Vector<cplx> & xy);
	};

	template <typename T, typename U> Constellation1<U> cconvert (Constellation1<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
