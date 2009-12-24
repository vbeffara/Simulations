/// @file Matrix.h
/// Definitions of a matrix template class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vb/MatrixStorage.h>

namespace vb {

  /** A template matrix class.
   *
   * It is just some bookkeeping logic on top of MatrixStorage, which 
   * does all the heavy lifting. Ideally MatrixStorage should never have 
   * to manage dynamic memory, except possibly for switching from one 
   * storage to the next ...
   */

  class Matrix {
    public:
      unsigned int width;      ///< The width of the matrix.
      unsigned int height;     ///< The height of the matrix.
      MatrixStorage *data;  ///< The underlying storage of the matrix.

      /** Standard constructor of a zero matrix.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       */

      Matrix (unsigned int h, unsigned int w) :
        width(w), height(h), data (new MatrixStorage_DiagSmallRank (h,w)) {}

      /** Constructor from a diagonal vector.
       *
       * The length of the vector should be larger than max(w,h), but 
       * the matrix does not have to be square.
       *
       * @param h The height of the matrix.
       * @param w The width of the matrix.
       * @param d The diagonal of the matrix.
       */

      Matrix (unsigned int h, unsigned int w, const Vector &d) :
        width(w), height(h), data (new MatrixStorage_DiagSmallRank (h,w,d)) {}

      /** Copy constructor.
       *
       * @param M The matrix to copy.
       */

      Matrix (const Matrix &M) : width(M.width), height(M.height), data(M.data->copy()) {}

      /** Assignment operator.
       *
       * @param M The matrix to copy.
       */

      Matrix &operator= (const Matrix &M) {
        if (&M != this) {
          width=M.width;
          height=M.height;
          delete data;
          data=M.data->copy();
        }
        return (*this);
      }

      /** Return a particular rntry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      double at (unsigned int i, unsigned int j) const {
        return data->at(i,j);
      }

      /** Return a particular rntry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       */

      double operator() (unsigned int i, unsigned int j) const {
        return this->at(i,j);
      }

      /** Set a particular entry of the matrix.
       *
       * @param i The line of the entry.
       * @param j The column of the entry.
       * @param t The new value of the entry.
       */

      Matrix &put (unsigned int i, unsigned int j, double t) { 
        MatrixStorage *tmp = data->put(i,j,t);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }

      /** Add another matrix to this one.
       *
       * @param M The matrix to add.
       */

      Matrix &operator+= (const Matrix &M) {
        MatrixStorage *tmp = data->add(M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }

      /** Subtract another matrix from this one.
       *
       * @param M The matrix to subtract.
       */

      Matrix &operator-= (const Matrix &M) {
        MatrixStorage *tmp = data->sub(M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }

      /** Right-multiply by another matrix.
       *
       * @param M The right multiplicator.
       */

      Matrix &operator*= (const Matrix &M) {
        MatrixStorage *tmp = data->mul_right (M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }

      /** Perform a rank-one update.
       *
       * This adds the rank-one matrix A^t.B to the current matrix.
       *
       * @param A The vertical vector of the update.
       * @param B The horizontal vector of the update.
       */

      Matrix &rank1update (const Vector &A, const Vector &B) {
        MatrixStorage *tmp = data->rank1update(A,B);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
  };

  /** Add two matrices.
   *
   * @param M The first matrix to add.
   * @param N The second matrix to add.
   */

  Matrix operator+ (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O+=N;
    return O;
  }

  /** Compute the difference between two matrices.
   *
   * @param M The first matrix.
   * @param N The second matrix.
   */

  Matrix operator- (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O-=N;
    return O;
  }

  /** Compute the product of two matrices.
   *
   * @param M The left matrix.
   * @param N The right matrix.
   */

  Matrix operator* (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O*=N;
    return O;
  }

  /** Compute the product of a matrix and a vector.
   *
   * @param M The matrix.
   * @param X The vector.
   */

  Vector operator* (const Matrix &M, const Vector &X) {
    return M.data->map_right(X);
  }

  /** Output a matrix to an output stream.
   *
   * @param os The output stream.
   * @param M  The matrix.
   */

  std::ostream &operator<< (std::ostream &os, const Matrix &M) {
    os << "[";
    for (unsigned int i=0; i<M.height; ++i) {
      os << "[";
      for (unsigned int j=0; j<M.width; ++j) {
        os << M(i,j);
        if (j < M.width-1) os << ",";
      }
      os << "]";
      if (i < M.height-1) os << ",";
    }
    return os << "]";
  }
}

#endif

