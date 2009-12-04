/// @file TriMatrix.h
/// Definitions for the vb::TriMatrix class

#ifndef __VB_TRIMATRIX_H
#define __VB_TRIMATRIX_H

#include <iostream>

namespace vb {

  /** A class for storing unbounded spatial data.
   *
   * Assumption : the matrix is in one of three states:
   *
   * (i)   Empty (tile == sub == NULL, default),
   * (ii)  Leaf (tile != NULL, sub == NULL),
   * (iii) Node (tile == NULL, sub != NULL).
   *
   * The square represented is (-size,size), and size==128 iff Leaf.
   */

  template <typename T> class TriMatrix {
    public:
      TriMatrix  (T empty = 0);
      ~TriMatrix ();

      T    get (int i, int j) const;
      void put (int i, int j, T t);

      T operator() (int i, int j) const { return get(i,j); };

      int size;

    private:
      int max (int i, int j) const { return (i>j?i:j); }
      void triple ();

      T _empty;
      T *tile;
      TriMatrix<T> *sub;
  };

  template <typename T> TriMatrix<T>::TriMatrix (T empty)
    : size(0), _empty(empty), tile(NULL), sub(NULL) { }

  template <typename T> TriMatrix<T>::~TriMatrix () {
    delete[] tile;
    delete[] sub;
  }

  template <typename T> T TriMatrix<T>::get (int i, int j) const {
    int target = max (abs(i), abs(j));
    if (target >= size) return _empty;

    // So now, it fits, target<size.

    if (size==128) return tile [(i+128) + 256*(j+128)];
    return _empty;
  }

  template <typename T> void TriMatrix<T>::triple () {
    if (size==0) return;

    TriMatrix<T> *tmp_sub = new TriMatrix<T> [9];

    for (int i=0; i<9; ++i) tmp_sub[i]._empty = _empty;

    tmp_sub[4].size = size;
    tmp_sub[4].tile = tile;
    tmp_sub[4].sub  = sub;

    tile = NULL;
    sub = tmp_sub;
    size = 3*size-1;
  }

  template <typename T> void TriMatrix<T>::put (int i, int j, T t) {
    std::cerr << "(" << i << "," << j << ") <- " << t << " " << std::endl;
    if (size==0) {
      tile = new T[256*256];
      for (int k=0; k<256*256; ++k) tile[k]=_empty;
      size = 128;
    }

    int target = max (abs(i), abs(j));
    while (size <= target) triple();

    // So now, it fits, target<size. Two cases:

    if (size==128) tile [(i+128) + 256*(j+128)] = t;
  }
}

#endif
