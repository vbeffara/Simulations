#pragma once
#include <vb/Constellation.h>
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> class Constellation0 : public Constellation<T> { public:
		using cplx = std::complex<T>;
		using Constellation<T>::b; using Constellation<T>::w; using Constellation<T>::f;

		Constellation0 (Hypermap M, Hub H, int n=3);
		Constellation0 ();

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

		Vector<cplx>	vec    	(const std::vector<Star<T>> & b, const std::vector<Star<T>> & w, const std::vector<Star<T>> & f)	const;
		Vector<T>   	coovec 	(const std::vector<Star<T>> & b, const std::vector<Star<T>> & w, const std::vector<Star<T>> & f)	const;
		void        	readcoo	(const Vector<T> & xy);
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		void	draw	(Image & img, bool smooth = true) const;

		cplx            	l = T(1);
		Polynomial<cplx>	P,Q;
	};

	template <typename T, typename U> Constellation0<U> cconvert (Constellation0<T> & C);

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation0<T> & C);
}
