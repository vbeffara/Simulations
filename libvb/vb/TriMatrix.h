/// @file TriMatrix.h
/// Definitions for the vb::TriMatrix class

#ifndef __VB_TRIMATRIX_H
#define __VB_TRIMATRIX_H

namespace vb {

  /** A class for storing unbounded spatial data.
   */

  template <typename T> class TriMatrix {
    public:
      TriMatrix (T _empty = T(0)) : empty (_empty) {};
      ~TriMatrix () {};

      T operator() (int i, int j) const { return empty; }
      void put (int i, int j, T t) { };

      /*************************************** Should be private below ! */

      T empty;
  };
}

#endif
