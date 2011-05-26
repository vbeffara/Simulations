/// @file Lattice.h
/// Everything to do with periodic lattices

#ifndef __VB_LATTICE_H
#define __VB_LATTICE_H

#include <vb/common.h>

namespace vb {

  class Lattice_move {
  public:
    Lattice_move (int _k, int _dx, int _dy) : k(_k), dx(_dx), dy(_dy) {}
    int k,dx,dy;
  };

  class Lattice_vertex {
  public:
    Lattice_vertex (int _x=0, int _y=0, int _k=0) : x(_x), y(_y), k(_k) {};

    Lattice_vertex & operator+= (const Lattice_move &m);
    Lattice_vertex   operator+  (const Lattice_move &m) const;

    int x,y,k;
  };

  class Lattice;
  typedef double (Lattice::*LatticeCostFunction)() const;

  class Lattice {
  public:
    Lattice (int _n, cpx _tau = cpx(0,1));

    cpx actual (cpx xy) const;
    cpx operator() (int x, int y, int k=0) const;
    cpx operator() (const Lattice_vertex &v) const;

    Lattice &bond (int k1, int k2, int dx=0, int dy=0);
    cpx shift (int k, int l) const;

    double energy () const;
    cpx shear () const;
    double relax_once ();
    void relax (double eps=0);
    cpx tau_rw () const;
    double cost_cp () const;

    double eval (LatticeCostFunction f) const { return (this->*f)(); };
    void optimize (LatticeCostFunction f, double eps=0);

    unsigned int n;                                ///< Number of vertices in a fundamental domain
    std::vector < std::vector<Lattice_move> > adj; ///< Adjacency lists
    cpx tau;                                       ///< Modulus of embedding in the complex plane
    std::vector<cpx> z;                            ///< Displacement of each vertex
    std::vector<double> r;                         ///< The radius of the disk at each point
  };

  template <typename T> class Lattice_rectangle {
  public:
    Lattice_rectangle (const Lattice &_L, int _w, int _h) : L(_L), w(_w), h(_h), data(w*h*L.n) {};

    T & operator() (int x, int y, int k) { return data[k + L.n*(x+w*y)]; }
    T & operator[] (const Lattice_vertex &v) { return (*this)(v.x,v.y,v.k); }

    const Lattice &L;
    const int w,h;
    std::vector<T> data;
  };
}

#endif
