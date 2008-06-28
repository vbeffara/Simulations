
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

  template <typename T> class Matrix {
    public:
      unsigned int width, height;
      MatrixStorage<T> *data;

      Matrix (unsigned int h, unsigned int w) : width(w), height(h), data (new MatrixStorage_DiagSmallRank<T> (h,w)) {}
      Matrix (unsigned int h, unsigned int w, const Vector<T> &d) : width(w), height(h), data (new MatrixStorage_DiagSmallRank<T> (h,w,d)) {}
      Matrix (const Matrix<T> &M) : width(M.width), height(M.height), data(M.data->copy()) {}

      Matrix operator= (const Matrix<T> &M) {
        if (&M != this) {
          width=M.width;
          height=M.height;
          delete data;
          data=M.data->copy();
        }
        return (*this);
      }

      T at (unsigned int i, unsigned int j) const {
        return data->at(i,j);
      }
      T operator() (unsigned int i, unsigned int j) const {
        return this->at(i,j);
      }

      Matrix &put (unsigned int i, unsigned int j, const T &t) { 
        MatrixStorage<T> *tmp = data->put(i,j,t);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
      Matrix &operator+= (const Matrix &M) {
        MatrixStorage<T> *tmp = data->add(M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
      Matrix &operator-= (const Matrix &M) {
        MatrixStorage<T> *tmp = data->sub(M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
      Matrix &operator*= (const Matrix &M) {
        MatrixStorage<T> *tmp = data->mul(M.data);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
      Matrix &rank1update (const Vector<T> &A, const Vector<T> &B) {
        MatrixStorage<T> *tmp = data->rank1update(A,B);
        if (data != tmp) { delete data; data = tmp; }
        return (*this);
      }
  };

  template <typename T> Matrix<T> operator+ (const Matrix<T> &M, const Matrix<T> &N) { Matrix<T> O=M; O+=N; return O; }
  template <typename T> Matrix<T> operator- (const Matrix<T> &M, const Matrix<T> &N) { Matrix<T> O=M; O-=N; return O; }
  template <typename T> Matrix<T> operator* (const Matrix<T> &M, const Matrix<T> &N) { Matrix<T> O=M; O*=N; return O; }

  template <typename T> Vector<T> operator* (const Matrix<T> &M, const Vector<T> &X) { return M.data->map_right(X); }

  template <typename T> std::ostream &operator<< (std::ostream &os, const Matrix<T> &M) {
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
