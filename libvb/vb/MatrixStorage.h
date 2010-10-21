/// @file
/// Definitions of various kinds of matrix storage.

#ifndef __VB_MATRIXSTORAGE_H
#define __VB_MATRIXSTORAGE_H

#include <boost/numeric/ublas/vector.hpp>

namespace vb {
  typedef boost::numeric::ublas::vector<double> Vector;

  /** Plain old matrix, storing each entry separately.
   *
   * Internally it is just a vector of lines, themselves being vectors. 
   * This is so that inner_prod() can be used for matrix 
   * multiplication.
   */

  class MatrixStorage_Plain {
    public:
      unsigned int width;
      unsigned int height;

      /// The lines of the matrix.

      std::vector<Vector> lines;

      /** The standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      MatrixStorage_Plain (unsigned int h, unsigned int w);

      virtual MatrixStorage_Plain *copy ();

      virtual MatrixStorage_Plain *compute ();

      virtual double at (unsigned int i, unsigned int j) const;

      virtual MatrixStorage_Plain *put (unsigned int i, unsigned int j, double t);

      /// Add another matrix to this one.
      virtual MatrixStorage_Plain *add (MatrixStorage_Plain *M);

      /// Subtract another matrix from this one.
      virtual MatrixStorage_Plain *sub (MatrixStorage_Plain *M);

      /// Right-multiply this matrix with another one.
      virtual MatrixStorage_Plain *mul_right (MatrixStorage_Plain *M);

      virtual MatrixStorage_Plain *rank1update (const Vector &A, const Vector &B);

      virtual Vector map_right (const Vector &X);
  };
}

#endif
