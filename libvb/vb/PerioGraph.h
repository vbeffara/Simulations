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

  const std::complex<double> PG_SHIFT[9] = {
    std::complex<double>(0,0),
    std::complex<double>(1,0),
    std::complex<double>(1,1),
    std::complex<double>(0,1),
    std::complex<double>(-1,1),
    std::complex<double>(-1,0),
    std::complex<double>(-1,-1),
    std::complex<double>(0,-1),
    std::complex<double>(1,-1)
  };

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
      std::complex<double> *Z;  ///< The embedding.
      std::complex<double> tau; ///< The modulus of the embedding.

      /** The standard constructor, builds a completely disconnected cell.
       *
       * @param nn The number of vertices in the cell.
       * @param dd Whether the graph is directed or not.
       */

      PerioCell (int nn, int dd = PG_UNDIRECTED) : n(nn), d(dd), tau(std::complex<double>(0.0,1.0)) {
        A = new unsigned short [n*n];
        for (int i=0; i<n*n; ++i) A[i]=0;

        Z = new std::complex<double> [n];
        for (int i=0; i<n; ++i) Z[i]=0.0;
      };

      /** Add an edge to the cell.
       *
       * @param i The origin of the edge.
       * @param j The target of the cell.
       * @param z The relative cell of the target.
       */

      void add_edge (int i, int j, int z = PG_HERE) {
        if (z <= 8) {
          A[n*i+j] |= (1<<z); 
          if (d==PG_UNDIRECTED) {
            int zz = z;
            if (z>0) zz = 1 + ((z+3)%8);
            A[n*j+i] |= (1<<zz);
          }
        } else std::cerr << "libvb: no such cell" << std::endl;
      }

      /** The L^2 energy, as embedded
       */

      double energy (void) {
        double t = 0;
        for (int i=0; i<n; ++i)
          for (int j=0; j<n; ++j)
            for (int k=0; k<=8; ++k)
              if (A[i*n+j] & (1<<k))
                t += abs((Z[j]+PG_SHIFT[k]-Z[i])*(Z[j]+PG_SHIFT[k]-Z[i]));
        return t/2;
      }

      /** One relaxation step
       */

      double relax_once (void) {
        for (int i=1; i<n; ++i) { // Z[0] is pinned for uniqueness
          std::complex<double> z(0,0);
          double degree=0;
          for (int j=0; j<n; ++j)
            //if (i!=j)
              for (int k=0; k<=8; ++k)
                if (A[i*n+j] & (1<<k)) {
                  z += Z[j]+PG_SHIFT[k];
                  degree += 1;
                }
          if (degree>0) Z[i] = z/degree;
        }
      }

      /** Full relaxation to the balanced embedding
       */

      double relax (double eps=0) {
        double s = energy();
        double t = s + eps + 1;
        while (t-s>eps) {
          t = s;
          relax_once();
          s = energy();
        }
      }

    protected:
      friend std::ostream &operator<< (std::ostream &os, PerioCell &C);    
  };

  std::ostream &operator<< (std::ostream &os, PerioCell &C) {
    os << "PerioCell(" << C.n << ",[";
    for (int i=0; i<C.n; ++i) {
      os << "<" << C.A[i*C.n];
      for (int j=1; j<C.n; ++j) os << "," << C.A[i*C.n+j];
      os << ">";
      if (i<C.n-1) os << ",";
    }
    os << "],[";
    for (int i=0; i<C.n; ++i) {
      os << C.Z[i];
      if (i<C.n-1) os << ",";
    }
    os << "]," << C.tau << ")";
    return os;
  };
}

#endif
