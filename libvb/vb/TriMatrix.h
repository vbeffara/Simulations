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
      TriMatrix  (T _empty = 0);
      ~TriMatrix ();

      T    get (int i, int j) const;
      void put (int i, int j, T t);

      T operator() (int i, int j) const { return get(i,j); };

    private:
      TriMatrix (T _empty, int base, T *center);

      T empty;
      int base;
      int stride;
      int mid;
      int size;
      TriMatrix<T> *sub;
      T* contents;
  };

  template <typename T> TriMatrix<T>::TriMatrix (T _empty) : empty(_empty) {
    base = (5*sizeof(TriMatrix<T>)) / sizeof(T);
    size = base;
    stride = 2*base+1;
    std::cerr << "Base tile is of width " << stride << std::endl;
    mid = (base+1)*stride;
    sub  = NULL;
    contents = new T[stride*stride];
    for (int i=0; i<stride*stride; ++i) contents[i]=empty;
  }

  template <typename T> TriMatrix<T>::~TriMatrix () {
    delete[] contents;
  }

  template <typename T> T TriMatrix<T>::get (int i, int j) const {
    return empty;
  }

  template <typename T> void TriMatrix<T>::put (int i, int j, T t) {
  }
}

#endif
