/// @file Lattice.h
/// Everything to do with periodic lattices

#ifndef __VB_LATTICE_H
#define __VB_LATTICE_H

#include <vb/common.h>

namespace vb {
  /// A way to describe a relative move in a vb::Lattice.
  class Lattice_move {
  public:
    /// The constructor.
    Lattice_move (int _k, int _dx, int _dy) : k(_k), dx(_dx), dy(_dy) {}

    int k;  ///< The vertex in the target fundamental domain.
    int dx; ///< The horizontal displacement.
    int dy; ///< The vertical displacement.
  };

  /// A utility class to point to a vertex in a libvb::Lattice or a libvb::LatticeRectangle.
  class Lattice_vertex {
  public:
    /// The constructor.
    Lattice_vertex (int _x=0, int _y=0, int _k=0) : x(_x), y(_y), k(_k) {};

    /// Move accordint to a vb::Lattice_move (in-place version).
    Lattice_vertex & operator+= (const Lattice_move &m);

    /// Move accordint to a vb::Lattice_move.
    Lattice_vertex   operator+  (const Lattice_move &m) const;

    int x; ///< The first coordinate of the fundamental domain containing the point.
    int y; ///< The second coordinate of the fundamental domain containing the point.
    int k; ///< The vertex identifying the point within its fundamental domain.
  };

  class Lattice;

  /// The kind of function to minimize to circle-pack a vb::Lattice.
  typedef double LatticeCostFunction (Lattice const &);

  /// The distance of a vb::Lattice to being circle-packed.
  double cost_cp (Lattice const &L);

  /// A class to represent periodic 2D lattices.
  class Lattice {
  public:

    /** The constructor.
     *
     * @param _n   The number of vertices in a fundamental domain.
     * @param _tau The default complex modulus of the embedding.
     */

    Lattice (int _n, cpx _tau = cpx(0,1));

    /// Map complex coordintes from lattice relative to absolute.
    cpx actual (cpx xy) const;

    /// Compute the affix of the designated vertex.
    cpx operator() (int x, int y, int k=0) const;

    /// Compute the affix of the designated vertex.
    cpx operator() (const Lattice_vertex &v) const;

    /// Add a bond to the lattice structure.
    Lattice &bond (int k1, int k2, int dx=0, int dy=0);

    /// Compute the displacement between two vertices in the same fundamental domain.
    cpx shift (int k, int l) const;

    /// The sum of bond square-lengths.
    double energy () const;

    /// The "distance" from isotropy of the SRW on the current embedding.
    cpx shear () const;

    /// Replace each vertex by the barycenter of its neighbors, in order.
    double relax_once ();

    /// Iterate relax_once until the energy is (approximately) minimized.
    void relax (double eps=0);

    /// Compute the modulus of the SRW embedding.
    cpx tau_rw () const;

    /// Minimize a cost function over possible embeddings.
    void optimize (LatticeCostFunction f, double eps=0);

    unsigned int n;                                ///< Number of vertices in a fundamental domain
    std::vector < std::vector<Lattice_move> > adj; ///< Adjacency lists
    cpx tau;                                       ///< Modulus of embedding in the complex plane
    std::vector<cpx> z;                            ///< Displacement of each vertex
    std::vector<double> r;                         ///< The radius of the disk at each point
  };

  /** A class representing a rectangle in a libvb::Lattice.
   *
   * The template parameter is the type of data to be put on each vertex
   * of the rectangle (typically a color or some kind of local information).
   */

  template <typename T> class Lattice_rectangle {
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

    Lattice_rectangle (const Lattice &_L, int _w, int _h) : L(_L), w(_w), h(_h), data(w*h*L.n) {};

    /// Return the label at a given vertex.
    T & operator() (int x, int y, int k) { return data[k + L.n*(x+w*y)]; }

    /// Return the label at a given vertex.
    T & operator[] (const Lattice_vertex &v) { return (*this)(v.x,v.y,v.k); }

    const Lattice &L;    ///< The adjacency structure of the graph.
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

#endif
