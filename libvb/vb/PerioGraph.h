/// @file PerioGraph.h
/// Helper class for periodic planar graphs

#ifndef __VB_PERIOGRAPH_H
#define __VB_PERIOGRAPH_H

#include <complex>

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

  typedef double cpx_base;
  typedef std::complex<cpx_base> cpx;

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

  class PerioCell {

  /** Description of one period of a periodic planar graph.
   *
   * It is essentially a finite graph, with an enriched adjacency matrix
   * to account for neighboring cells. The restriction is that a cell
   * has only 8 neighboring cells, excluding longer edges - but this can
   * be taken into account by taking a larger cell.
   */

    public:
      int n;                    ///< The number of points in the cell.
      int d;                    ///< Whether the graph is directed or not.
      unsigned short *A;        ///< The adjacency matrix.
      cpx *Z;                   ///< The embedding.
      cpx_base *R;              ///< The radii (for circle packing).
      cpx tau;                  ///< The modulus of the embedding.

      /** The standard constructor, builds a completely disconnected cell.
       *
       * @param nn The number of vertices in the cell.
       * @param dd Whether the graph is directed or not.
       */

      PerioCell (int nn, int dd = PG_UNDIRECTED) : n(nn), d(dd), tau(cpx(0.0,1.0)) {
        A = new unsigned short [n*n];
        for (int i=0; i<n*n; ++i) A[i]=0;

        Z = new cpx [n];
        for (int i=0; i<n; ++i) Z[i]=0.0;

        R = new cpx_base [n];
        for (int i=0; i<n; ++i) R[i]=1.0;
      };

      /** Add an edge to the cell.
       *
       * @param i The origin of the edge.
       * @param j The target of the cell.
       * @param z The relative cell of the target.
       */

      void add_edge (int i, int j, int z = PG_HERE) {
        if ((z <= 8) && (i<n) && (j<n)) {
          A[n*i+j] |= (1<<z); 
          if (d==PG_UNDIRECTED) {
            int zz = z;
            if (z>0) zz = 1 + ((z+3)%8);
            A[n*j+i] |= (1<<zz);
          }
        } else std::cerr << "libvb: no such edge!" << std::endl;
      }

      /** The L^2 energy, as embedded
       */

      cpx_base energy (void) {
        cpx_base t = 0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if (A[i*n+j] & (1<<k))
                t += norm (Z[j]+PG_SHIFT[k]-Z[i]);
        return t/2;
      }

      /** One relaxation step
       */

      cpx_base relax_once (void) {
        cpx_base diff=0;
        for (int i=1; i<n; ++i) { // Z[0] is pinned for uniqueness
          cpx z(0,0);
          cpx_base degree=0;
          for (int j=0; j<n; ++j)
            //if (i!=j)
              for (int k=0; k<=8; ++k)
                if (A[i*n+j] & (1<<k)) {
                  z += Z[j]+PG_SHIFT[k];
                  degree += 1;
                }
          if (degree>0) {
            diff += abs(Z[i]*degree - z);
            Z[i] = z/degree;
          }
        }
        return diff;
      }

      /** Full relaxation to the balanced embedding
       */

      void relax (cpx_base eps=0) {
        while (relax_once()>eps) { }
      }

      /** Random-walk shear = sum of e^2
       */

      cpx rw_shear () {
        cpx t=0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if (A[i*n+j] & (1<<k)) {
                cpx u = Z[j]+PG_SHIFT[k]-Z[i];
                cpx e = u.real() + tau*u.imag();
                t += e*e;
              }
        return t/(cpx_base)2;
      }

      /** Compute tau_RW (semi-numericly)
       */

      cpx rw_tau () {
        cpx_base a=0, b=0, c=0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if (A[i*n+j] & (1<<k)) {
                cpx u = Z[j]+PG_SHIFT[k]-Z[i];
                a += u.imag()*u.imag();
                b += 2*u.real()*u.imag();
                c += u.real()*u.real();
              }
        if (a==(cpx_base)0.0) return cpx(0,0);
        cpx delta = b*b - 4*a*c;
        cpx t = (-b + sqrt(delta))/(a*(cpx_base)2);
        if (t.imag()<0) t = conj(t);
        tau = t;
        return t;
      }

      /** Difference from being a circle packing
       */

      cpx_base cp_cost () {
        cpx_base t=0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            if (i != j)
              for (int k=0; k<=8; ++k)
                if (A[i*n+j] & (1<<k)) {
                  cpx u = Z[j]+PG_SHIFT[k]-Z[i];
                  cpx e = u.real() + tau*u.imag();
                  cpx_base r = abs(e);
                  t += abs(r-R[i]-R[j]);
                }
        return t/(cpx_base)2;
      }

    protected:
      friend std::ostream &operator<< (std::ostream &os, PerioCell &C);    
  };

  void draw_segment (std::ostream &os, cpx z1, cpx z2, cpx tau=cpx(0,1)) {
    z1 = actual (z1,tau);
    z2 = actual (z2,tau);

    os << "draw (" << z1 << "--" << z2 << ");" << std::endl;
  }

  std::ostream &operator<< (std::ostream &os, PerioCell &C) {
    os << "unitsize(72);" << std::endl;
    for (int i=0; i<C.n; ++i)
      for (int j=0; j<C.n; ++j)
        for (int k=0; k<=8; ++k)
          if (C.A[i*C.n+j] & (1<<k)) {
            for (int l=0; l<=8; ++l)
              draw_segment (os,C.Z[i] + PG_SHIFT[l],
                               C.Z[j] + PG_SHIFT[l] + PG_SHIFT[k],
                               C.tau);
          }
    return os;
  };
}

#endif
