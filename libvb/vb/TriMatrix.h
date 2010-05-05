/// @file
/// Definitions for the vb::TriMatrix class

#ifndef __VB_TRIMATRIX_H
#define __VB_TRIMATRIX_H

#include <vb/common.h>

#ifndef TRIMATRIX_BSIZE
/// The size of a fundamental block in the storage structure.
#define TRIMATRIX_BSIZE 128
#endif

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
   * The square represented is (-size,size), and size==TRIMATRIX_BSIZE 
   * iff Leaf.
   */

  template <typename T> class TriMatrix {
    public:
      /** The default creator.
       *
       * The argument is the default value returned by get() for unset 
       * entries, it defaults to 0 (meaning that the underlying type T 
       * must be initializable from 0 ...)
       *
       * @param e The value of an empty entry.
       */

      TriMatrix  (T e = 0);

      /** Get the contents of an entry in the matrix.
       *
       * If the corresponding entry is not set yet, return the default 
       * value as specified to the constructor.
       *
       * @param i The first coordinate of the entry.
       * @param j The second coordinate of the entry.
       */

      T get (int i, int j) const;

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

      int                          size;
      int                          sub_size;
      int                          sub_shift;
      T                            empty;
      std::vector < T >            tile;
      std::vector < TriMatrix<T> > sub;
  };

  template <typename T> TriMatrix<T>::TriMatrix (T e)
    : size(0), sub_size(0), sub_shift(0), empty(e) { }

  template <typename T> T TriMatrix<T>::get (int i, int j) const {
    int target = max (abs(i), abs(j));
    if (target >= size) return empty;

    // So now, it fits, target<size.

    if (size==TRIMATRIX_BSIZE)
      return tile [(2*TRIMATRIX_BSIZE+1)*TRIMATRIX_BSIZE + i + 2*TRIMATRIX_BSIZE*j];

    // So we are a node, we need to recurse.

    int index = 4;

    if      (i >=   sub_size) { index += 1; i -= sub_shift; }
    else if (i <= - sub_size) { index -= 1; i += sub_shift; }
    if      (j >=   sub_size) { index += 3; j -= sub_shift; }
    else if (j <= - sub_size) { index -= 3; j += sub_shift; }

    return sub[index].get(i,j);
  }

  template <typename T> void TriMatrix<T>::triple () {
    if (size==0) return;

    std::vector < TriMatrix<T> > tmp_sub (9, empty);

    tmp_sub.swap (sub);
    tmp_sub.swap (sub[4].sub);

    sub[4].tile.swap (tile);
    sub[4].size      = size;
    sub[4].sub_size  = sub_size;
    sub[4].sub_shift = sub_shift;

    sub_size  = size;
    size      = 3*size-1;
    sub_shift = size - sub_size;
  }

  template <typename T> void TriMatrix<T>::put (int i, int j, T t) {
    if (size==0) {
      tile.resize (2*TRIMATRIX_BSIZE*2*TRIMATRIX_BSIZE, empty);
      size = TRIMATRIX_BSIZE;
    }

    int target = max (abs(i), abs(j));
    while (size <= target) triple();

    // So now, it fits, target<size. Two cases:

    if (size==TRIMATRIX_BSIZE)
      tile [(2*TRIMATRIX_BSIZE+1)*TRIMATRIX_BSIZE + i + 2*TRIMATRIX_BSIZE*j] = t;
    else {
      int index = 4;

      if      (i >=   sub_size) { index += 1; i -= sub_shift; }
      else if (i <= - sub_size) { index -= 1; i += sub_shift; }
      if      (j >=   sub_size) { index += 3; j -= sub_shift; }
      else if (j <= - sub_size) { index -= 3; j += sub_shift; }

      sub[index].put(i,j,t);
    }
  }
}

#endif
