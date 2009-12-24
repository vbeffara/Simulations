/// @file Matrix.h
/// Definitions of a matrix class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vb/MatrixStorage.h>

namespace vb {
  /** A matrix class.
   *
   * It is just some bookkeeping logic on top of MatrixStorage, which 
   * does all the heavy lifting. Ideally MatrixStorage should never have 
   * to manage dynamic memory, except possibly for switching from one 
   * storage to the next ...
   */

  class Matrix {
    public:
      unsigned int width;   ///< The width of the matrix.
      unsigned int height;  ///< The height of the matrix.
      MatrixStorage *data;  ///< The underlying storage of the matrix.

      /** Standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      Matrix (unsigned int h, unsigned int w);

      /** Constructor from a diagonal vector.
       *
       * The length of the vector should be larger than max(w,h), but 
       * the matrix does not have to be square.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       * @param d The diagonal of the matrix.
       */

      Matrix (unsigned int h, unsigned int w, const Vector &d);

      /** Copy constructor.
       *
       * @param M The matrix to copy.
       */

      Matrix (const Matrix &M);

      /** Destructor.
       */

      ~Matrix ();

      /** Assignment operator.
       *
       * @param M The matrix to copy.
       */

      Matrix &operator= (const Matrix &M);

      /** Return a particular rntry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      double at (unsigned int i, unsigned int j) const;

      /** Return a particular rntry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      double operator() (unsigned int i, unsigned int j) const;

      /** Set a particular entry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       * @param t The new value of the entry.
       */

      Matrix & put (unsigned int i, unsigned int j, double t);

      /** Add another matrix to this one.
       *
       * @param M The matrix to add.
       */

      Matrix & operator+= (const Matrix &M);

      /** Subtract another matrix from this one.
       *
       * @param M The matrix to subtract.
       */

      Matrix & operator-= (const Matrix &M);

      /** Right-multiply by another matrix.
       *
       * @param M The right multiplicator.
       */

      Matrix & operator*= (const Matrix &M);

      /** Perform a rank-one update.
       *
       * This adds the rank-one matrix A^t.B to the current matrix.
       *
       * @param A The vertical vector of the update.
       * @param B The horizontal vector of the update.
       */

      Matrix & rank1update (const Vector &A, const Vector &B);
  };

  /** Add two matrices.
   *
   * @param M The first matrix to add.
   * @param N The second matrix to add.
   */

  Matrix operator+ (const Matrix &M, const Matrix &N);

  /** Compute the difference between two matrices.
   *
   * @param M The first matrix.
   * @param N The second matrix.
   */

  Matrix operator- (const Matrix &M, const Matrix &N);

  /** Compute the product of two matrices.
   *
   * @param M The left matrix.
   * @param N The right matrix.
   */

  Matrix operator* (const Matrix &M, const Matrix &N);

  /** Compute the product of a matrix and a vector.
   *
   * @param M The matrix.
   * @param X The vector.
   */

  Vector operator* (const Matrix &M, const Vector &X);

  /** Output a matrix to an output stream.
   *
   * @param os The output stream.
   * @param M  The matrix.
   */

  std::ostream &operator<< (std::ostream &os, const Matrix &M);
}

#endif
