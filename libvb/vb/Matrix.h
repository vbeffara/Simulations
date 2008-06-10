
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

      Matrix<T> &operator*= (const Matrix<T> &M) {
        (*this) = (*this) * M;
        return (*this);
      }
  };

  template <class T> Matrix<T> operator+ (const Matrix<T> &A, const Matrix<T> &B) {
    Matrix<T> S = A;
    S += B;
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
}

#endif
