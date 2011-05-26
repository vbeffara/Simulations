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

  class Lattice {
  public:
    Lattice (int _n, cpx _tau = cpx(0,1));

    cpx operator() (int x, int y, int k=0) const;
    cpx operator() (const Lattice_vertex &v) const;

    void bond (int k1, int k2, int dx=0, int dy=0);
    cpx shift (int k, int l) const;

    double energy () const;
    cpx shear () const;
    double relax_once ();
    void relax (double eps=0);

    unsigned int n;                                ///< Number of vertices in a fundamental domain
    std::vector < std::vector<Lattice_move> > adj; ///< Adjacency lists
    cpx tau;                                       ///< Modulus of embedding in the complex plane
    std::vector<cpx> z;                            ///< Displacement of each vertex
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

/*
 *   cpx actual (cpx z, cpx tau = cpx(0,1)) {
 *     return z.real() + tau * z.imag();
 *   }
 *
 *       cpx rw_tau () {
 *         double a=0, b=0, c=0;
 *         for (int i=0; i<n; ++i)
 *           for (int j=0; j<n; ++j)
 *             for (int k=0; k<=8; ++k)
 *               if ((*this)(i,j,k)) {
 *                 cpx u = (*this)(j)+PG_SHIFT[k]-(*this)(i);
 *                 a += u.imag()*u.imag();
 *                 b += 2*u.real()*u.imag();
 *                 c += u.real()*u.real();
 *               }
 *         if (a==(double)0.0) return cpx(0,0);
 *         cpx delta = b*b - 4*a*c;
 *         cpx t = (-b + sqrt(delta))/(a*(double)2);
 *         if (t.imag()<0) t = conj(t);
 *         tau = t;
 *         return t;
 *       }
 *
 *       cpx cp_tau (double eps = 0) {
 *         return optimize(cost_cp);
 *       }
 *
 *       cpx optimize (double func(PerioCell), double eps = 0) {
 *         double cost = func(*this);
 *         double old_cost = cost + eps + 1;
 *         double tmp_cost = cost;
 *         while (old_cost - cost > eps) {
 *           old_cost = cost;
 *           double delta = sqrt(cost)/10;
 *
 *           tau += cpx(delta,0); tmp_cost = func(*this);
 *           if (tmp_cost < cost) cost = tmp_cost;
 *           else {
 *             tau -= cpx(2*delta,0); tmp_cost = func(*this);
 *             if (tmp_cost < cost) cost = tmp_cost;
 *             else tau += cpx(delta,0);
 *           }
 *
 *           tau += cpx(0,delta); tmp_cost = func(*this);
 *           if (tmp_cost < cost) cost = tmp_cost;
 *           else {
 *             tau -= cpx(0,2*delta); tmp_cost = func(*this);
 *             if (tmp_cost < cost) cost = tmp_cost;
 *             else tau += cpx(0,delta);
 *           }
 *
 *           for (int i=0; i<n; ++i) {
 *             if (i>0) {
 *               (*this)(i) += cpx(delta,0); tmp_cost = func(*this);
 *               if (tmp_cost < cost) cost = tmp_cost;
 *               else {
 *                 (*this)(i) -= cpx(2*delta,0); tmp_cost = func(*this);
 *                 if (tmp_cost < cost) cost = tmp_cost;
 *                 else (*this)(i) += cpx(delta,0);
 *               }
 *
 *               (*this)(i) += cpx(0,delta); tmp_cost = func(*this);
 *               if (tmp_cost < cost) cost = tmp_cost;
 *               else {
 *                 (*this)(i) -= cpx(0,2*delta); tmp_cost = func(*this);
 *                 if (tmp_cost < cost) cost = tmp_cost;
 *                 else (*this)(i) += cpx(0,delta);
 *               }
 *             }
 *
 *             R[i] += delta; tmp_cost = func(*this);
 *             if (tmp_cost < cost) cost = tmp_cost;
 *             else {
 *               R[i] -= 2*delta; tmp_cost = func(*this);
 *               if (tmp_cost < cost) cost = tmp_cost;
 *               else R[i] += delta;
 *             }
 *           }
 *         }
 *
 *         return tau;
 *       }
 *   };
 *
 *   double cost_cp (PerioCell C) {
 *     double t=0;
 *     for (int i=0; i<C.n; ++i)
 *       for (int j=0; j<C.n; ++j)
 *         for (int k=0; k<=8; ++k)
 *           if (C(i,j,k)) {
 *             cpx e = actual ( C(j)+PG_SHIFT[k]-C(i), C.tau );
 *             double d = sqrt ( e.real()*e.real() + e.imag()*e.imag() );
 *             double r = C.R[i]+C.R[j];
 *             t += (d-r)*(d-r);
 *           }
 *     return t/2.0;
 *   }
 *
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
