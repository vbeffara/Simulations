/// @file
/// Definitions of a matrix class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace vb {
  typedef boost::numeric::ublas::vector<double> Vector;

  class Matrix : public boost::numeric::ublas::matrix<double> {
    public:
      unsigned int width;   ///< The width of the matrix.
      unsigned int height;  ///< The height of the matrix.

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
}

#endif
