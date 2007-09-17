/// @file PerioGraph.h
/// Helper class for periodic planar graphs

#ifndef __VB_PERIOGRAPH_H
#define __VB_PERIOGRAPH_H

#include <vb/Figure.h>

#define PG_HERE         0
#define PG_EAST         1
#define PG_NORTHEAST    2
#define PG_NORTH        3
#define PG_NORTHWEST    4
#define PG_WEST         5
#define PG_SOUTHWEST    6
#define PG_SOUTH        7
#define PG_SOUTHEAST    8

#define PG_UNDIRECTED   0
#define PG_DIRECTED     1

namespace vb {
  /** The displacements for the various lattice shifts.
   */

  const cpx PG_SHIFT[9] = {
    cpx(0,0),
    cpx(1,0),
    cpx(1,1),
    cpx(0,1),
    cpx(-1,1),
    cpx(-1,0),
    cpx(-1,-1),
    cpx(0,-1),
    cpx(1,-1)
  };

  /** Translate square to actual embedding
   */

  cpx actual (cpx z, cpx tau = cpx(0,1)) {
    return z.real() + tau * z.imag();
  }

  class PerioCell;
  double cost_cp (PerioCell);

  /** Description of one period of a periodic planar graph.
   *
   * It is essentially a finite graph, with an enriched adjacency matrix
   * to account for neighboring cells. The restriction is that a cell
   * has only 8 neighboring cells, excluding longer edges - but this can
   * be taken into account by taking a larger cell.
   */

  class PerioCell {

    public:
      int n;                    ///< The number of points in the cell.
      int d;                    ///< Whether the graph is directed or not.
      std::vector<short> A;     ///< The adjacency matrix.
      std::vector<cpx> Z;       ///< The embedding.
      std::vector<double> R;    ///< The radii (for circle packing).
      cpx tau;                  ///< The modulus of the embedding.

      /** The standard constructor, builds a completely disconnected cell.
       *
       * @param nn The number of vertices in the cell.
       * @param dd Whether the graph is directed or not.
       */

      PerioCell (int nn, int dd = PG_UNDIRECTED) : n(nn), d(dd), tau(cpx(0.0,1.0)) {
        for (int i=0; i<n*n; ++i) A.push_back(0);
        for (int i=0; i<n; ++i) Z.push_back(0.0);
        for (int i=0; i<n; ++i) R.push_back(0.0);
      };

      /** Access the point locations via an operator.
       */

      cpx &operator() (int i) {
        return Z[i];
      }

      /** Access the adjacency matrix via an operator.
       */

      short &operator() (int i, int j) {
        return A[i*n+j];
      }

      /** Add an edge to the cell.
       *
       * @param i The origin of the edge.
       * @param j The target of the cell.
       * @param z The relative cell of the target.
       */

      void add_edge (int i, int j, int z = PG_HERE) {
        if ((z <= 8) && (i<n) && (j<n)) {
          (*this)(i,j) |= (1<<z); 
          if (d==PG_UNDIRECTED) {
            int zz = z;
            if (z>0) zz = 1 + ((z+3)%8);
            (*this)(j,i) |= (1<<zz);
          }
        } else std::cerr << "libvb: no such edge!" << std::endl;
      }

      /** The L^2 energy, as embedded
       */

      double energy (void) {
        double t = 0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if ((*this)(i,j) & (1<<k))
                t += norm ((*this)(j)+PG_SHIFT[k]-(*this)(i));
        return t/2;
      }

      /** One relaxation step
       */

      double relax_once (void) {
        double diff=0;
        for (int i=1; i<n; ++i) { // Z[0] is pinned for uniqueness
          cpx z(0,0);
          double degree=0;
          for (int j=0; j<n; ++j)
            //if (i!=j)
              for (int k=0; k<=8; ++k)
                if ((*this)(i,j) & (1<<k)) {
                  z += (*this)(j)+PG_SHIFT[k];
                  degree += 1;
                }
          if (degree>0) {
            diff += abs((*this)(i)*degree - z);
            (*this)(i) = z/degree;
          }
        }
        return diff;
      }

      /** Full relaxation to the balanced embedding
       */

      void relax (double eps=0) {
        while (relax_once()>eps) { }
      }

      /** Random-walk shear = sum of e^2
       */

      cpx rw_shear () {
        cpx t=0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if ((*this)(i,j) & (1<<k)) {
                cpx u = (*this)(j)+PG_SHIFT[k]-(*this)(i);
                cpx e = u.real() + tau*u.imag();
                t += e*e;
              }
        return t/(double)2;
      }

      /** Compute tau_RW (semi-numericly)
       */

      cpx rw_tau () {
        double a=0, b=0, c=0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if ((*this)(i,j) & (1<<k)) {
                cpx u = (*this)(j)+PG_SHIFT[k]-(*this)(i);
                a += u.imag()*u.imag();
                b += 2*u.real()*u.imag();
                c += u.real()*u.real();
              }
        if (a==(double)0.0) return cpx(0,0);
        cpx delta = b*b - 4*a*c;
        cpx t = (-b + sqrt(delta))/(a*(double)2);
        if (t.imag()<0) t = conj(t);
        tau = t;
        return t;
      }

      /** Relax to a circle packing.
       */

      cpx cp_tau (double eps = 0) {
        return optimize(cost_cp);
      }

      /** Find the minimum of a function over embeddings.
       *
       * @param func The function that should be minimized.
       * @param eps Stop when making steps less than this.
       */

      cpx optimize (double func(PerioCell), double eps = 0) {
        double cost = func(*this);
        double old_cost = cost + eps + 1;
        double tmp_cost = cost;
        while (old_cost - cost > eps) {
          old_cost = cost;
          double delta = sqrt(cost)/10;

          tau += cpx(delta,0); tmp_cost = func(*this);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            tau -= cpx(2*delta,0); tmp_cost = func(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else tau += cpx(delta,0);
          }

          tau += cpx(0,delta); tmp_cost = func(*this);
          if (tmp_cost < cost) cost = tmp_cost;
          else {
            tau -= cpx(0,2*delta); tmp_cost = func(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else tau += cpx(0,delta);
          }

          for (int i=0; i<n; ++i) {
            if (i>0) {
              (*this)(i) += cpx(delta,0); tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else {
                (*this)(i) -= cpx(2*delta,0); tmp_cost = func(*this);
                if (tmp_cost < cost) cost = tmp_cost;
                else (*this)(i) += cpx(delta,0);
              }

              (*this)(i) += cpx(0,delta); tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else {
                (*this)(i) -= cpx(0,2*delta); tmp_cost = func(*this);
                if (tmp_cost < cost) cost = tmp_cost;
                else (*this)(i) += cpx(0,delta);
              }
            }

            R[i] += delta; tmp_cost = func(*this);
            if (tmp_cost < cost) cost = tmp_cost;
            else {
              R[i] -= 2*delta; tmp_cost = func(*this);
              if (tmp_cost < cost) cost = tmp_cost;
              else R[i] += delta;
            }
          }
        }

        return tau;
      }

    protected:
      friend std::ostream &operator<< (std::ostream &os, PerioCell &C);    
  };

  /** Difference from being a circle packing
  */

  double cost_cp (PerioCell C) {
    double t=0;
    for (int i=0; i<C.n; ++i)
      for (int j=0; j<C.n; ++j)
        for (int k=0; k<=8; ++k)
          if (C(i,j) & (1<<k)) {
            cpx e = actual ( C(j)+PG_SHIFT[k]-C(i), C.tau );
            double d = sqrt ( e.real()*e.real() + e.imag()*e.imag() );
            double r = C.R[i]+C.R[j];
            t += (d-r)*(d-r);
          }
    return t/2.0;
  }
}

#endif
