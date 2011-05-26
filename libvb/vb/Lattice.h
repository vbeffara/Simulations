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

  class Lattice {
  public:
    Lattice (int _n, cpx _tau = cpx(0,1));
    void bond (int k1, int k2, int dx=0, int dy=0);

    unsigned int n;                                ///< Number of vertices in a fundamental domain
    std::vector < std::vector<Lattice_move> > adj; ///< Adjacency lists
    cpx tau;                                       ///< Modulus of embedding in the complex plane
    std::vector<cpx> z;                            ///< Displacement of each vertex
  };

  class Lattice_vertex {
  public:
    Lattice_vertex (const Lattice &_L, int _x=0, int _y=0, int _k=0) : L(_L), x(_x), y(_y), k(_k) {};

    Lattice_vertex & operator=  (const Lattice_vertex &v);
    Lattice_vertex & operator+= (const Lattice_move &m);
    Lattice_vertex   operator+  (const Lattice_move &m) const;

    operator cpx ();

    const Lattice &L;
    int x,y,k;
  };

  template <typename T> class Lattice_rectangle {
  public:
    Lattice_rectangle (const Lattice &_L, int _w, int _h) : L(_L), w(_w), h(_h), data(w*h*L.n) {};

    T & operator[] (const Lattice_vertex &v) { return data[v.k + L.n*(v.x + w*v.y)]; }
    T & operator() (int x, int y, int k) { return (*this)[Lattice_vertex(L,x,y,k)]; }

    const Lattice &L;
    const int w,h;
    std::vector<T> data;
  };

/*
 *   /\** Template for a decorated cell, to be used in vb::PerioGraph.
 *    *
 *    * It will only work if T implements a copy constructor and can be
 *    * output to a std::ostream via operator<< overload.
 *    *\/
 *   template <class T> class DecoratedCell {
 *     private:
 *       int n;                ///< The number of vertices.
 *       PerioCell *C;         ///< The base pattern.
 *       std::vector<T> D;     ///< The decorations.
 *
 *     public:
 *       /\** Constructor from a PerioCell and a default element.
 *        *\/
 *       DecoratedCell (PerioCell &CC, T t) : n(CC.n), C(&CC) {
 *         for (int i=0; i<n; ++i) D.push_back(t);
 *       }
 *
 *       /\** Access the labels via an operator.
 *        *\/
 *       T &operator() (int i) {
 *         return D[i];
 *       }
 *   };
 *
 *   /\** The main class template for a (decorated) periodic graph.
 *    *\/
 *   template <class T> class PerioGraph {
 *     private:
 *       PerioCell C;                               ///< The repeated pattern.
 *       std::vector < DecoratedCell<T> > field;    ///< The data.
 *
 *     public:
 *       int wd;                                    ///< The number of periods in the horizontal direction.
 *       int ht;                                    ///< The number of periods in the vertical direction.
 *
 *       /\** The constructor from a PerioCell.
 *        *
 *        * @param width  The width (in periods) of the graph.
 *        * @param height The height (in periods) of the graph.
 *        * @param cell   The repeated cell.
 *        * @param t      The default value of the label.
 *        *\/
 *       PerioGraph (int width, int height, PerioCell cell, T t) : C(cell), wd(width), ht(height) {
 *         for (int i=0; i<wd; ++i)
 *           for (int j=0; j<ht; ++j)
 *             field.push_back(DecoratedCell<T> (C,t));
 *       }
 *
 *       /\** Access the labels via an operator.
 *        *\/
 *       T &operator() (int x, int y, int i) {
 *         return field[wd*x+y](i);
 *       }
 *
 *       /\** Nice output to a stream.
 *        *\/
 *       std::ostream &printOn (std::ostream &os) {
 *         for (int i=0; i<wd; ++i)
 *           for (int j=0; j<ht; ++j)
 *             field[wd*i+j].printOn(os, actual(cpx(i,j),C.tau));
 *         return os;
 *       }
 *   };
 */
}

#endif
