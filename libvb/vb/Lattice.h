#pragma once /// \file
#include <vb/common.h>

namespace vb {
	class Lattice_place {
	public:
		Lattice_place (coo _z=0, int _k=0) : z(_z), k(_k) {};

		Lattice_place & operator+= (const Lattice_place &m);
		Lattice_place   operator+  (const Lattice_place &m) const;

		coo z; ///< The coordinates of the fundamental domain containing the point.
		int k; ///< The vertex identifying the point within its fundamental domain.
	};

	class Lattice {
	public:
		Lattice (int _n, cpx _tau = cpx(0,1)) : n(_n), adj(_n), tau(_tau), z(_n), r(_n) {};

		cpx actual (cpx xy) const;                    	///< Map complex coordinates from lattice relative to absolute.
		cpx operator() (coo z, int k=0) const;        	///< Compute the affix of the designated vertex.
		cpx operator() (const Lattice_place &v) const;	///< Compute the affix of the designated vertex.

		Lattice &bond (int k1, int k2, coo dz = coo(0,0));	///< Add a bond to the lattice structure.
		cpx shift (int k, int l) const;                   	///< Compute the displacement between two vertices in the same fundamental domain.

		double energy () const;                                  	///< The sum of bond square-lengths.
		cpx shear () const;                                      	///< The "distance" from isotropy of the SRW on the current embedding.
		double relax_once ();                                    	///< Replace each vertex by the barycenter of its neighbors, in order.
		void relax (double eps=0);                               	///< Iterate relax_once until the energy is (approximately) minimized.
		void optimize (double f (Lattice const &), double eps=0);	///< Minimize a cost function over possible embeddings.

		cpx tau_rw () const;	///< Compute the modulus of the SRW embedding.

		unsigned int n;                                	///< Number of vertices in a fundamental domain
		std::vector < std::vector<Lattice_place> > adj;	///< Adjacency lists
		cpx tau;                                       	///< Modulus of embedding in the complex plane
		std::vector<cpx> z;                            	///< Displacement of each vertex
		std::vector<double> r;                         	///< The radius of the disk at each point
	};

	template <typename T> class Lattice_dressed : public Lattice {
	public:
		Lattice_dressed (int n, cpx tau = cpx(0,1))	: Lattice(n,tau),	labels(n) {};
		Lattice_dressed (Lattice const &L)         	: Lattice(L),    	labels(L.n) {};
		std::vector<T> labels;
	};

	double cost_cp (Lattice const &L);

	/** A class representing a rectangle in a libvb::Lattice.
	 *
	 * The template parameter is the type of data to be put on each vertex
	 * of the rectangle (typically a color or some kind of local information).
	 */

	template <typename T> class Lattice_rectangle : public Lattice {
	public:

		/** The constructor.
		 *
		 * The rectangle it produces has _L.n * _w * _h vertices, each one
		 * carrying as a label an instance of class T.
		 *
		 * @param _L The lattice structure.
		 * @param _w The width of the rectangle, measured in terms of the fundamental domain.
		 * @param _h The height of the rectangle, measured in terms of the fundamental domain.
		 */

		Lattice_rectangle (const Lattice &_L, int _w, int _h) : Lattice(_L), w(_w), h(_h), data(w*h*n) {};

		/// Return the label at a given vertex.
		T & operator() (coo z, int k) { return data[k + n*(real(z)+w*imag(z))]; }

		/// Return the label at a given vertex.
		T & operator[] (const Lattice_place &v) { return (*this)(v.z,v.k); }

		const int w;         ///< The width of the rectangle.
		const int h;         ///< The height of the rectangle.
		std::vector<T> data; ///< The labels.
	};

	Lattice Z2();  ///< The square lattice.
	Lattice H();   ///< The hexagonal lattice.
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
