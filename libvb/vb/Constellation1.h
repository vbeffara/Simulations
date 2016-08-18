#pragma once
#include <vb/Constellation.h>
#include <vb/Elliptic.h>
#include <vb/Hypermap.h>

namespace vb {
	template <typename T> class Constellation1 : public Constellation<T> { public:
		using typename Constellation<T>::cplx;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f; using Constellation<T>::p;
		using Constellation<T>::findn; using Constellation<T>::cost; using Constellation<T>::dim;

		Constellation1                      	(const Hypermap & M, const Hub & H);
		template <typename U> Constellation1	(const Constellation1<U> & C);

		cplx	operator()	(cplx z)	const;

		std::pair<cplx,cplx>	bounds	()	const;

		cplx tau () const { return p[0]; }

		int        	dx,dy;
		Elliptic<T>	E { q_<T>(I_<T>()) };

	private:
		Vector<cplx>	vec    	()	const;
		void        	readvec	(const Vector<cplx> & xy);

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

		void	from_points	();
		void	normalize  	();
		void	shift      	(cplx z);
	};

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation1<T> & C);
}
