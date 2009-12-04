/// @file TriMatrix.h
/// Definitions for the vb::TriMatrix class

#ifndef __VB_TRIMATRIX_H
#define __VB_TRIMATRIX_H

#include <vb/common.h>

namespace vb {

  /** A class for storing unbounded spatial data.
   *
   * The structure grows dynamically like a 9-ary tree, tripling its 
   * size as often as needed, but this is transparent to the user. Just 
   * use get() and put() and focus on something more interesting!
   *
   * The whole class is a template, so the code is inline and 
   * performance should be quite good, but I didn't test it properly, 
   * maybe something more efficient can be done ...
   */

   /* Assumption : the matrix is in one of three states:
   *
   * (i)   Empty (tile == sub == NULL, default),
   * (ii)  Leaf (tile != NULL, sub == NULL),
   * (iii) Node (tile == NULL, sub != NULL).
   *
   * The square represented is (-size,size), and size==128 iff Leaf.
   */

  template <typename T> class TriMatrix {
    public:
      /** The default creator.
       *
       * The argument is the default value returned by get() for unset 
       * entries, it defaults to 0 (meaning that the underlying type T 
       * must be initializable from 0 ...)
       *
       * @param empty The value of an empty entry.
       */

      TriMatrix  (T empty = 0);
      ~TriMatrix ();

      /** Get the contents of an entry in the matrix.
       *
       * If the corresponding entry is not set yet, return the default 
       * value as specified to the constructor.
       *
       * @param i The first coordinate of the entry.
       * @param j The second coordinate of the entry.
       */

      T    get (int i, int j) const;

      /** Set the contents of an entry in the matrix.
       *
       * If the corresponding entry is out of bounds, grow the structure 
       * accordingly (even if t is the default value, but that might 
       * change eventually).
       *
       * @param i The first coordinate of the entry.
       * @param j The second coordinate of the entry.
       * @param t The value to store in the entry.
       */

      void put (int i, int j, T t);

      /** Get the contents of an entry in the matrix.
       *
       * If the corresponding entry is not set yet, return the default 
       * value as specified to the constructor.
       *
       * This is exactly equivalent to get().
       *
       * @param i The first coordinate of the entry.
       * @param j The second coordinate of the entry.
       */

      T operator() (int i, int j) const { return get(i,j); };

    private:
      void triple ();

      int size;
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

    // So we are a node, we need to recurse.

    int sub_size = (size+1)/3; // TODO : Store this somewhere to save computations ?
    int sub_shift = size - sub_size;
    int index = 4;

    if (i >=   sub_size) { index += 1; i -= sub_shift; }
    if (i <= - sub_size) { index -= 1; i += sub_shift; }
    if (j >=   sub_size) { index += 3; j -= sub_shift; }
    if (j <= - sub_size) { index -= 3; j += sub_shift; }

    return sub[index].get(i,j);
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
    if (size==0) {
      tile = new T[256*256];
      for (int k=0; k<256*256; ++k) tile[k]=_empty;
      size = 128;
    }

    int target = max (abs(i), abs(j));
    while (size <= target) triple();

    // So now, it fits, target<size. Two cases:

    if (size==128) {
      tile [(i+128) + 256*(j+128)] = t;
    } else {
      int sub_size = (size+1)/3; // TODO : Store this somewhere to save computations ?
      int sub_shift = size - sub_size;
      int index = 4;

      if (i >=   sub_size) { index += 1; i -= sub_shift; }
      if (i <= - sub_size) { index -= 1; i += sub_shift; }
      if (j >=   sub_size) { index += 3; j -= sub_shift; }
      if (j <= - sub_size) { index -= 3; j += sub_shift; }

      sub[index].put(i,j,t);
    }
  }
}

#endif
