/// @file TriMatrix.h
/// Definitions for the vb::TriMatrix class

#ifndef __VB_TRIMATRIX_H
#define __VB_TRIMATRIX_H

#include <iostream>

namespace vb {

  /** A class for storing unbounded spatial data.
   */

  template <typename T> class TriMatrix {
    public:
      TriMatrix (T _empty = 0) : empty(_empty) {
        base = (5*sizeof(TriMatrix<T>)) / sizeof(T);
        size = base;
        stride = 2*base+1;
        std::cerr << "Base tile is of width " << stride << std::endl;
        mid = (base+1)*stride;
        sub  = NULL;
        contents = new T[stride*stride];
        for (int i=0; i<stride*stride; ++i) contents[i]=empty;
      };

      ~TriMatrix () {
        delete[] contents;
      };

      T operator() (int i, int j) const { return empty; }
      void put (int i, int j, T t) { };

      /*************************************** Should be private below ! */

      T empty;
      int base;
      int stride;
      int mid;
      int size;
      TriMatrix<T> *sub;
      T* contents;
  };
}

#endif
