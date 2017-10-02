#pragma once

#include <vb/coo.h>

namespace vb {
	class Lattice_place {
	public:
		Lattice_place             	(coo _z={0,0}, int _k=0) : z(_z), k(_k)	{};
		Lattice_place & operator+=	(const Lattice_place &m)           	{ z+=m.z; k=m.k; return *this; };
		Lattice_place   operator+ 	(const Lattice_place &m) const     	{ Lattice_place tmp (z,k); tmp += m; return tmp; };

		coo z; int k;
	};

	class Lattice {
	public:
		Lattice (int _n, cpx _tau = cpx(0,1)) : n(_n), adj(_n), tau(_tau), z(_n), r(_n) {};

		cpx      	actual    	(cpx xy) const                             	{ return real(xy) + tau*imag(xy); };
		cpx      	operator()	(coo xy, int k=0) const                    	{ return actual(cpx(xy.x,xy.y) + z[k]); };
		cpx      	operator[]	(const Lattice_place &v) const             	{ return actual(cpx(v.z.x,v.z.y) + z[v.k]); }
		cpx      	shift     	(int k, int l) const;                      	///< Displacement between vertices in the same fundamental domain.
		double   	energy    	() const;                                  	///< The sum of bond square-lengths.
		cpx      	shear     	() const;                                  	///< The "distance" from isotropy of the SRW.
		double   	relax_once	();                                        	///< Replace each vertex by the barycenter of its neighbors.
		void     	relax     	(double eps=0);                            	///< Iterate relax_once until the energy is  minimized.
		void     	optimize  	(double f (Lattice const &), double eps=0);	///< Minimize a cost function over possible embeddings.
		Lattice &	bond      	(int k1, int k2, coo dz = coo(0,0));       	///< Add a bond.
		cpx      	tau_rw    	() const;                                  	///< Compute the modulus of the SRW embedding.

		unsigned n;                                	///< Number of vertices in a fundamental domain
		std::vector < std::vector<Lattice_place> > adj;	///< Adjacency lists
		cpx tau;                                       	///< Modulus of embedding in the complex plane
		std::vector<cpx> z; std::vector<double> r;
	};

	double cost_cp (Lattice const &L);

	Lattice Z2();  ///< The square lattice.
	Lattice Hx();  ///< The hexagonal lattice.
	Lattice H2();  ///< The hexagonal lattice, rectangular embedding.
	Lattice T();   ///< The triangular lattice.
	Lattice T2();  ///< The triangular lattice, rectangular embedding.
	Lattice K();   ///< The Kagome lattice;
	Lattice K2();  ///< The Kagome lattice, rectangular embedding.
	Lattice SO();  ///< The square-octogon lattice.
	Lattice Z2C(); ///< The square lattice with centered faces.
	Lattice G67(); ///< The "\sqrt{6/7}" graph.
	Lattice SV();  ///< The Shabat-Voevodsky graph.
	Lattice C5();  ///< The C5 graph (automorphism group is Z^2).
}
