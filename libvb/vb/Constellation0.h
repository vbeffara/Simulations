#pragma once
#include <vb/Constellation.h>
#include <vb/Hypermap.h>
#include <boost/optional.hpp>

namespace vb {
	template <typename T> class Constellation0 : public Constellation<T> { public:
		using cplx = std::complex<T>;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f; using Constellation<T>::p;
		using Constellation<T>::findn; using Constellation<T>::cost; using Constellation<T>::dim;

		Constellation0                      	(const Hypermap & M, const Hub & H);
		template <typename U> Constellation0	(const Constellation0<U> & C);

		cplx	operator()	(cplx z)	const;

		std::pair<cplx,cplx>	bounds	()	const;

		void	belyi	();	// does too many things at once

		boost::optional<Hypermap>	explore	()	const; // Recover the hypermap


	private:
		Vector<cplx>	vec    	()	const;
		void        	readvec	(const Vector<cplx> & xy);

		Vector<cplx>	vcost   	()	const;
		Matrix<cplx>	jacvcost	()	const;


		cplx	logder   	(cplx z, int k=0)          	const;
		void	linear   	(cplx u, cplx v = cplx(0));	// move the points, recompute P and Q, don't touch l
		void	normalize	();                        	// choose l to make ones ones
		void	make_l_1 	();                        	// rescale to make l equal to 1
		void	make_c_0 	();                        	// shift everybody to make sum(black)=0
		void	make_p_1 	();                        	// try to have reasonable scaling
	};







	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation0<T> & C);
}
