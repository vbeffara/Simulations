
/// @file Matrix.h
/// Definitions of a matrix template class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>

namespace vb {
  template <class T> class Matrix {
    public:
      int lines, columns;
      std::vector < std::vector <T> > data;

      Matrix (int l, int c = 1) : lines(l), columns(c) {
        for (int i=0; i<lines; ++i) data.push_back(std::vector<T>(columns));
      }

      Matrix<T> &operator= (const Matrix<T> &M) {
        if (this == &M) return *this;

        if ((lines == M.lines) && (columns == M.columns)) {
          for (int i=0; i<lines; ++i)
            for (int j=0; j<columns; ++j)
              data[i][j] = M.data[i][j];
        } else {
          lines = M.lines;
          columns = M.columns;
          data = M.data;
        }

        return *this;
      }

      Matrix<T> &operator+= (const Matrix<T> &M) {
        if ((lines != M.lines) || (columns != M.columns))
          throw std::runtime_error("vb::Matrix : wrong dimension.");
        for (int i=0; i<lines; ++i)
          for (int j=0; j<columns; ++j)
            data[i][j] += M.data[i][j];
        return *this;
      }

      Matrix<T> &operator-= (const Matrix<T> &M) {
        if ((lines != M.lines) || (columns != M.columns))
          throw std::runtime_error("vb::Matrix : wrong dimension.");
        for (int i=0; i<lines; ++i)
          for (int j=0; j<columns; ++j)
            data[i][j] -= M.data[i][j];
        return *this;
      }

      Matrix<T> &operator*= (const Matrix<T> &M) {
        (*this) = (*this) * M;
        return (*this);
      }

      Matrix<T> &operator*= (T l) {
        for (int i=0; i<lines; ++i)
          for (int j=0; j<columns; ++j)
            data[i][j] *= l;
        return (*this);
      }

      Matrix<T> &operator/= (T l) {
        for (int i=0; i<lines; ++i)
          for (int j=0; j<columns; ++j)
            data[i][j] /= l;
        return (*this);
      }
  };

  template <class T> Matrix<T> operator- (const Matrix<T> &M) {
    Matrix<T> B = M;
    for (int i=0; i<B.lines; ++i)
      for (int j=0; j<B.columns; ++j)
        B.data[i][j] = - B.data[i][j];
    return B;
  }

  template <class T> Matrix<T> operator+ (const Matrix<T> &A, const Matrix<T> &B) {
    Matrix<T> S = A;
    S += B;
    return S;
  }

  template <class T> Matrix<T> operator- (const Matrix<T> &A, const Matrix<T> &B) {
    Matrix<T> S = A;
    S -= B;
    return S;
  }

  template <class T> Matrix<T> operator* (const Matrix<T> &A, const Matrix<T> &B) {
    if (A.columns != B.lines) throw std::runtime_error("vb::Matrix : wrong dimension.");

    Matrix<T> P (A.lines, B.columns);
    for (int i=0; i<A.lines; ++i) {
      for (int j=0; j<B.columns; ++j) {
        P.data[i][j] = A.data[i][0] * B.data[0][j];
        for (int k=1; k<A.columns; ++k) {
          P.data[i][j] += A.data[i][k] * B.data[k][j]; 
        }
      }
    }
    return P;
  }

  template <class T> std::ostream &operator<< (std::ostream &os, const Matrix<T> &M) {
    os << "matrix(" << M.lines << "," << M.columns << ") [";
    for (int i=0; i<M.lines; ++i) {
      os << "[";
      for (int j=0; j<M.columns; ++j) {
        os << M.data[i][j];
        if (j < M.columns-1) os << ",";
      }
      os << "]";
      if (i < M.lines-1) os << ",";
    }
    return os << "]";
  }

  template <class T> Matrix<T> operator* (T l, const Matrix<T> &M) {
    Matrix<T> P = M; P *= l;
    return P;
  }

  template <class T> Matrix<T> operator* (const Matrix<T> &M, T l) {
    return l*M;
  }

  template <class T> Matrix<T> operator/ (const Matrix<T> &M, T l) {
    Matrix<T> P = M; P /= l;
    return P;
  }

  template <class T> Matrix<T> transpose (const Matrix<T> M) {
    Matrix<T> P (M.columns, M.lines);
    for (int i=0; i<P.lines; ++i)
      for (int j=0; j<P.columns; ++j)
        P.data[i][j] = M.data[j][i];
    return P;
  }

  template <class T> T scalar_product (const Matrix<T> &A, const Matrix<T> &B) {
    if ((A.lines != B.lines) || (A.columns != B.columns))
          throw std::runtime_error("vb::Matrix : wrong dimension.");

    T t = A.data[0][0] * B.data[0][0];
    for (int i=0; i<A.lines; ++i)
      for (int j=0; j<A.columns; ++j)
        if ((i!=0) || (j!=0))
          t += A.data[i][j] * B.data[i][j];
    return t;
  }

  template <class T> T norm_squared (const Matrix<T> &M) {
    return scalar_product(M,M);
  }
}

#endif
