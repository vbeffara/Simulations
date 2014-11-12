#pragma once
#include <vb/Bitmap.h>
#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/LinearAlgebra.h>
#include <vb/NumberTheory.h>

namespace vb {
	template <typename T> class Constellation;

	template <typename T> class CPixel { public:
		CPixel (Constellation<T> * C_, std::complex<T> z_ = T(0)) : C(C_), z(z_) {}
		operator Color() { if (C->slow || prng.bernoulli(.01)) c = imag((*C)(C->center + C->scale*z))>0 ? WHITE : BLUE; return c; }
		Constellation<T> * C;
		std::complex<T> z;
		Color c;
	};

	template <typename T> class Constellation { public:
		using cplx = std::complex<T>;

		Constellation (Hypermap M, Hub H, int n=3, bool d=true);
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

		void	find 	();
		void	findm	();
		void	findn	();

		Vector<cplx>	vec    	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f)	const;
		Vector<T>   	coovec 	(const std::vector<cplx> & b, const std::vector<cplx> & w, const std::vector<cplx> & f)	const;
		void        	readcoo	(const Vector<T> & xy);
		void        	readvec	(const Vector<cplx> & xy);

		T	fg (const Vector<T> & xy, Vector<T> & df);

		void	show	();

		std::vector<cplx>    	b,w,f;
		std::vector<unsigned>	bd,wd,fd;

		cplx             	l = T(1);
		Polynomial<cplx> 	P,Q;
		Bitmap<CPixel<T>>	*img = 0;
		cplx center; T scale; bool slow=false;
	};

	template <typename T, typename U> Constellation<U> cconvert (Constellation<T> & C);

	template <typename T> T Constellation_fg (const Vector<T> & xy, Vector<T> & df, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		return C->fg(xy,df);
	}

	template <typename T> void Constellation_cb (const Vector<T> &, T f, void * c) {
		Constellation<T> * C = (Constellation<T> *) c;
		static T er (-1); T out = f;
		if ((out<er)||(er<T(0))) { std::cerr << out << "          \r"; er = out; }
		if (C->img) C->img->step();
	}

	template <typename T> std::ostream & operator<< (std::ostream & os, const Constellation<T> & C);
}
