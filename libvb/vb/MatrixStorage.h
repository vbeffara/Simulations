/// @file
/// Definitions of various kinds of matrix storage.

#ifndef __VB_MATRIXSTORAGE_H
#define __VB_MATRIXSTORAGE_H

#include <boost/numeric/ublas/vector.hpp>

namespace vb {
  typedef boost::numeric::ublas::vector<double> Vector;

  /** Abstract base class for a general matrix storage class.
   *
   * All matrix-matrix operations return a pointer to a (possibly 
   * different) MatrixStorage subclass, it is the responsibility of the 
   * surrounding vb::Matrix to juggle between them. Not sure that this 
   * is such a wonderful way of doing it, but it works pretty well and 
   * is transparent to the user.
   *
   * @todo Add in-place versions of as many things as possible.
   */

  class MatrixStorage {
    public:
      unsigned int width;  ///< The width of the matrix;
      unsigned int height; ///< The height of the matrix.

      /** Standard constructor.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage (unsigned int h, unsigned int w);

      /// Produce a copy of the object and return it.

      virtual MatrixStorage *copy () =0;

      /// Compute each of the entry, converting to MatrixStorage_Plain.

      virtual MatrixStorage *compute () =0;

      /** Return the entry at the given coordinates.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      virtual double at (unsigned int i, unsigned int j) const =0;

      /** Set the value of a particular matrix entry.
       *
       * @param i The line of the entry.
       * @param j The line of the entry.
       * @param t The new value of the entry.
       */

      virtual MatrixStorage *put (unsigned int i, unsigned int j, double t) =0;

      /** Add a MatrixStorage to the current one.
       *
       * NB There is no add(const vb::Matrix) because it is not the 
       * point ... MatrixStorage does not know about Matrix. It's 
       * simpler this way.
       *
       * @param M The MatrixStorage to add.
       */

      virtual MatrixStorage *add (MatrixStorage *M) =0;

      /** Subtract a MatrixStorage from the current one.
       *
       * NB There is no sub(const vb::Matrix) because it is not the 
       * point ... MatrixStorage does not know about Matrix. It's 
       * simpler this way.
       *
       * @param M The MatrixStorage to subtract.
       */

      virtual MatrixStorage *sub (MatrixStorage *M) =0;

      /** Right-multiply the current matrix by another one.
       *
       * @param M The (right) multiplicator.
       */

      virtual MatrixStorage *mul_right (MatrixStorage *M) =0;

      /** Make a rank-one update of the matrix.
       *
       * This adds the rank-one matrix A^t.B to the current matrix.
       *
       * @param A The column vector of the rank-one matrix.
       * @param B The line vector of the rank-one matrix.
       */

      virtual MatrixStorage *rank1update (const Vector &A, const Vector &B) =0;

      /** Map a vector by the current matrix.
       *
       * @param X The vector to map.
       */

      virtual Vector map_right (const Vector &X) =0;
  };

  /** Plain old matrix, storing each entry separately.
   *
   * Internally it is just a vector of lines, themselves being vectors. 
   * This is so that inner_prod() can be used for matrix 
   * multiplication.
   */

  class MatrixStorage_Plain : public MatrixStorage {
    public:
      /// The lines of the matrix.

      std::vector<Vector> lines;

      /** The standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage_Plain (unsigned int h, unsigned int w);

      virtual MatrixStorage *copy ();

      virtual MatrixStorage *compute ();

      virtual double at (unsigned int i, unsigned int j) const;

      virtual MatrixStorage *put (unsigned int i, unsigned int j, double t);

      /// Add another matrix to this one.
      virtual MatrixStorage *add (MatrixStorage *M);

      /// Subtract another matrix from this one.
      virtual MatrixStorage *sub (MatrixStorage *M);

      /// Right-multiply this matrix with another one.
      virtual MatrixStorage *mul_right (MatrixStorage *M);

      virtual MatrixStorage *rank1update (const Vector &A, const Vector &B);

      virtual Vector map_right (const Vector &X);
  };
}

#endif
