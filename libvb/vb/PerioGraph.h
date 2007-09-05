/// @file PerioGraph.h
/// Helper class for periodic planar graphs

#ifndef __VB_PERIOGRAPH_H
#define __VB_PERIOGRAPH_H

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
  class PerioCell {

  /** Description of one period of a periodic planar graph.
   *
   * It is essentially a finite graph, with an enriched adjacency matrix
   * to account for neighboring cells. The restriction is that a cell
   * has only 8 neighboring cells, excluding longer edges - but this can
   * be taken into account by taking a larger cell.
   */

    public:
      int n;                   ///< The number of points in the cell.
      int d;                   ///< Whether the graph is directed or not.
      unsigned short *A;       ///< The adjacency matrix.

      /** The standard constructor, builds a completely disconnected cell.
       *
       * @param nn The number of vertices in the cell.
       * @param dd Whether the graph is directed or not.
       */

      PerioCell (int nn, int dd = PG_UNDIRECTED) : n(nn), d(dd) {
        A = new unsigned short [n*n];
        for (int i=0; i<n*n; ++i) A[i]=0;
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
    os << "])";
    return os;
  };
}

#endif
