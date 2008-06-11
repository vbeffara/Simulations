
/// @file Matrix.h
/// Definitions of a matrix template class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>

namespace vb {
  template <class T> class Vector;

  template <class T> class Matrix {
    public:
      unsigned int lines, columns;
      std::vector < std::vector <T> > data;

      Matrix (int l, int c = 1) : lines(l), columns(c) {
        for (unsigned int i=0; i<lines; ++i) data.push_back(std::vector<T>(columns));
      }

      Matrix (const Vector<T> &V) : lines(V.size()), columns(1) {
        for (unsigned int i=0; i<V.size(); ++i) {
          Vector<T> t;
          t.push_back(V[i]);
          data.push_back(t);
        }
      }

      Matrix<T> &operator= (const Matrix<T> &M) {
        if (this == &M) return *this;

        if ((lines == M.lines) && (columns == M.columns)) {
          for (unsigned int i=0; i<lines; ++i)
            for (unsigned int j=0; j<columns; ++j)
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
        for (unsigned int i=0; i<lines; ++i)
          for (unsigned int j=0; j<columns; ++j)
            data[i][j] += M.data[i][j];
        return *this;
      }

      Matrix<T> &operator-= (const Matrix<T> &M) {
        if ((lines != M.lines) || (columns != M.columns))
          throw std::runtime_error("vb::Matrix : wrong dimension.");
        for (unsigned int i=0; i<lines; ++i)
          for (unsigned int j=0; j<columns; ++j)
            data[i][j] -= M.data[i][j];
        return *this;
      }

      Matrix<T> &operator*= (const Matrix<T> &M) {
        (*this) = (*this) * M;
        return (*this);
      }

      Matrix<T> &operator*= (T l) {
        for (unsigned int i=0; i<lines; ++i)
          for (unsigned int j=0; j<columns; ++j)
            data[i][j] *= l;
        return (*this);
      }

      Matrix<T> &operator/= (T l) {
        for (unsigned int i=0; i<lines; ++i)
          for (unsigned int j=0; j<columns; ++j)
            data[i][j] /= l;
        return (*this);
      }
  };

  template <class T> Matrix<T> operator- (const Matrix<T> &M) {
    Matrix<T> B = M;
    for (unsigned int i=0; i<B.lines; ++i)
      for (unsigned int j=0; j<B.columns; ++j)
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
    for (unsigned int i=0; i<A.lines; ++i) {
      for (unsigned int j=0; j<B.columns; ++j) {
        P.data[i][j] = A.data[i][0] * B.data[0][j];
        for (unsigned int k=1; k<A.columns; ++k) {
          P.data[i][j] += A.data[i][k] * B.data[k][j]; 
        }
      }
    }
    return P;
  }

  template <class T> std::ostream &operator<< (std::ostream &os, const Matrix<T> &M) {
    os << "[";
    for (unsigned int i=0; i<M.lines; ++i) {
      os << "[";
      for (unsigned int j=0; j<M.columns; ++j) {
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
    for (unsigned int i=0; i<P.lines; ++i)
      for (unsigned int j=0; j<P.columns; ++j)
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

  /*******************************/

  template <class T> class Vector : public std::vector<T> {
    public:
      Vector (int size) : std::vector<T> (size) { };

      Vector<T> &operator+= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] += O[i];
        return *this;
      }

      Vector<T> &operator-= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] -= O[i];
        return *this;
      }

      Vector<T> &operator*= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] *= l;
        return *this;
      }

      Vector<T> &operator/= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] /= l;
        return *this;
      }

      Vector<T> operator- () const {
        Vector O(this->size());
        for (unsigned int i=0; i<this->size(); ++i) O[i] = -(*this)[i];
        return O;
      }
  };

  template <class T> Vector<T> operator* (const Matrix<T> &A, const Vector<T> &X) {
    if (A.columns != X.size()) throw std::runtime_error("vb::Matrix : wrong dimension.");

    Vector<T> Y(A.lines);
    for (unsigned int i=0; i<A.lines; ++i) {
      Y[i] = A.data[i][0] * X[0];
      for (unsigned int j=1; j<A.columns; ++j)
        Y[i] += A.data[i][j] * X[j];
    }
    return Y;
  }

  template <class T> Matrix<T> operator* (const Vector<T> &X, const Matrix<T> &A) {
    if (A.lines != 1) throw std::runtime_error("vb::Matrix : wrong dimension.");

    Matrix<T> B (X.size(),A.columns);
    for (unsigned int i=0; i<X.size(); ++i)
      for (unsigned int j=0; j<A.columns; ++j)
        B.data[i][j] = X[i]*A.data[0][j];
    return B;
  }

  template <class T> T scalar_product (const Vector<T> &X, const Vector<T> &Y) {
    if (X.size() != Y.size()) throw std::runtime_error("vb::Matrix : wrong dimension.");

    T t = X[0]*Y[0];
    for (unsigned int i=1; i<X.size(); ++i) t += X[i]*Y[i];
    return t;
  }

  template <class T> Vector<T> operator* (const T &l, const Vector<T> &X) {
    Vector<T> Y = X;
    Y *= l;
    return Y;
  }

  template <class T> Vector<T> operator* (const Vector<T> &X, const T &l) {
    return l*X;
  }

  template <class T> Vector<T> operator/ (const Vector<T> &X, const T &l) {
    Vector<T> Y = X;
    Y /= l;
    return Y;
  }

  template <class T> Matrix<T> transpose (const Vector<T> &X) {
    Matrix<T> Y(1,X.size());
    for (unsigned int i=0; i<X.size(); ++i) Y.data[0][i] = X[i];
    return Y;
  }

  template <class T> Vector<T> operator+ (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z += Y;
    return Z;
  }

  template <class T> Vector<T> operator- (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z -= Y;
    return Z;
  }

  template <class T> std::ostream &operator<< (std::ostream &os, const Vector<T> &V) {
    os << "[";
    for (unsigned int i=0; i<V.size(); ++i) {
      os << V[i];
      if (i < V.size()-1) os << ",";
    }
    os << "]";
    return os;
  }

  template <class T> Matrix<T> aTb (const Vector<T> &A, const Vector<T> &B) {
    Matrix<T> M(A.size(), B.size());
    for (unsigned int i=0; i<A.size(); ++i)
      for (unsigned int j=0; j<B.size(); ++j)
        M.data[i][j] = A[i]*B[j];
    return M;
  }
}

#endif
