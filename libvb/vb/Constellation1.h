#pragma once
#include <vb/Constellation.h>
#include <vb/Elliptic.h>
#include <vb/Hub.h>
#include <vb/Hypermap.h>

namespace vb {
	template <typename T> class Constellation1 : public Constellation<T> { public:
		using cplx = std::complex<T>;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f; using Constellation<T>::p;
		using Constellation<T>::findn; using Constellation<T>::cost;

		Constellation1                      	(const Hypermap & M, const Hub & H);
		template <typename U> Constellation1	(const Constellation1<U> & C);

		void	from_points	();
		cplx	operator() 	(cplx z)	const;

		Elliptic<T>	E { q_(cplx(0,1)) };

		cplx tau () const { return p[0]; }

		std::pair<cplx,cplx>	bounds	()	const;

		int	dx,dy,d;

	private:
		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;
		Matrix<cplx>	jacnum  	();

		cplx	reduce   	(cplx z)	const;
		cplx	logderp  	(cplx z, int k) const;
		cplx	logderp_z	(cplx z, int k) const;
		cplx	logderp_q	(cplx z, int k) const;
		cplx	logderp_t	(cplx z, int k) const;
		cplx	logder   	(cplx z, int k)	const;
		cplx	logder_z 	(cplx z, int k)	const;
		cplx	logder_t 	(cplx z, int k)	const;

		void	normalize	();
		void	shift    	(cplx z);

		Vector<cplx>	vec    	()	const;
		void        	readvec	(const Vector<cplx> & xy);
	};

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
