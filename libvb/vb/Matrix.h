
/// @file Matrix.h
/// Definitions of a matrix template class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vector>
#include <iostream>
#include <stdexcept>

namespace vb {
  template <typename T> class Vector;

  template <typename T> class MatrixStorage {
    public:
      unsigned int width, height;

      MatrixStorage (unsigned int h, unsigned int w) : width(w), height(h) {}
      virtual ~MatrixStorage () {}

      virtual MatrixStorage<T> *copy () =0;
      virtual MatrixStorage<T> *compute () =0;

      virtual T at (unsigned int i, unsigned int j) const =0;
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) =0;
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) =0;
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) =0;
      virtual Vector<T> map_right (const Vector<T> &X) =0;

  };

  template <typename T> class MatrixStorage_Plain : public MatrixStorage<T> {
    public:
      std::vector< Vector<T> > lines;

      MatrixStorage_Plain (unsigned int h, unsigned int w) : MatrixStorage<T> (h,w), lines (std::vector< Vector<T> > (h,Vector<T>(w))) { }
      virtual ~MatrixStorage_Plain () {}

      virtual MatrixStorage<T> *copy () {
        return new MatrixStorage_Plain<T> (*this);
      }
      virtual MatrixStorage<T> *compute () { return this; }

      virtual T at (unsigned int i, unsigned int j) const {
        return lines[i][j];
      }
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) {
        lines[i][j] = t; return this;
      }
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += M->at(i,j);
        return this;
      }
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] -= M->at(i,j);
        return this;
      }
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) {
        MatrixStorage_Plain<T> *tmp = new MatrixStorage_Plain<T> (this->height, M->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<M->width; ++j) {
            tmp->lines[i][j] = lines[i][0] * M->at(0,j);
            for (unsigned int k=1; k<this->width; ++k) 
              tmp->lines[i][j] += lines[i][k] * M->at(k,j);
          }
        }
        return tmp;
      }
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) {
        for (unsigned int i=0; i<this->height; ++i)
          for (unsigned int j=0; j<this->width; ++j)
            lines[i][j] += A[i]*B[j];
        return this;
      }
      virtual Vector<T> map_right (const Vector<T> &X) {
        Vector<T> Y(this->height);
        for (unsigned int i=0; i<this->height; ++i)
          Y[i] = scalar_product (lines[i],X);
        return Y;
      }
  };

  template <typename T> class MatrixStorage_DiagSmallRank : public MatrixStorage<T> {
    public:
      Vector<T> diag;
      Vector< std::pair< Vector<T>,Vector<T> > > updates;

      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w) : MatrixStorage<T> (h,w) {
        diag = Vector<T> (h);
        for (unsigned int i=0; i<h; ++i) diag[i]=0;
      }
      MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w, const Vector<T> &d) : MatrixStorage<T> (h,w), diag(d) { }
      virtual ~MatrixStorage_DiagSmallRank () {}

      virtual MatrixStorage<T> *copy () {
        return new MatrixStorage_DiagSmallRank (*this);
      }
      MatrixStorage<T> *compute () {
        MatrixStorage_Plain<T> *tmp = new MatrixStorage_Plain<T> (this->height, this->width);
        for (unsigned int i=0; i<this->height; ++i) {
          for (unsigned int j=0; j<updates.size(); ++j)
            tmp->lines[i] += updates[j].first[i] * updates[j].second;
        }
        for (unsigned int i=0; i<(this->height<this->width?this->height:this->width); ++i)
          tmp->lines[i][i] += diag[i];
        return tmp;
      }

      virtual T at (unsigned int i, unsigned int j) const {
        T tmp = 0;
        if (i==j) tmp += diag[i];
        for (unsigned int k=0; k<updates.size(); ++k)
          tmp += updates[k].first[i] * updates[k].second[j];
        return tmp;
      }
      virtual MatrixStorage<T> *put (unsigned int i, unsigned int j, const T &t) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->put (i,j,t);
        return tmp;
      }
      virtual MatrixStorage<T> *add (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->add(M);
        return tmp;
      }
      virtual MatrixStorage<T> *sub (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->sub(M);
        return tmp;
      }
      virtual MatrixStorage<T> *mul (MatrixStorage<T> *M) {
        MatrixStorage<T> *tmp = this->compute();
        tmp->mul(M);
        return tmp;
      }
      virtual MatrixStorage<T> *rank1update (const Vector<T> &A, const Vector<T> &B) {
        updates.push_back (std::pair< Vector<T>,Vector<T> > (A,B));
        if (updates.size() > this->height/10) {
          return this->compute();
        } else {
          return this;
        }
      }
      virtual Vector<T> map_right (const Vector<T> &X) {
        Vector<T> tmp(this->width);
        for (unsigned int i=0; i<(this->width<this->height?this->width:this->height); ++i) tmp[i] = diag[i]*X[i];
        for (unsigned int i=0; i<updates.size(); ++i) tmp += updates[i].first * scalar_product(updates[i].second,X);
        return tmp;
      }
  };

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

  /***************************************************/

  template <typename T> class Vector : public std::vector<T> {
    public:
      Vector (int size = 0) : std::vector<T> (size) { };
      Vector (int size, const T &t) : std::vector<T> (size,t) { };

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

  template <typename T> T scalar_product (const Vector<T> &X, const Vector<T> &Y) {
    if (X.size() != Y.size()) throw std::runtime_error("vb::Matrix : wrong dimension.");

    T t = X[0]*Y[0];
    for (unsigned int i=1; i<X.size(); ++i) t += X[i]*Y[i];
    return t;
  }

  template <typename T> Vector<T> operator* (const T &l, const Vector<T> &X) {
    Vector<T> Y = X;
    Y *= l;
    return Y;
  }

  template <typename T> Vector<T> operator* (const Vector<T> &X, const T &l) {
    return l*X;
  }

  template <typename T> Vector<T> operator/ (const Vector<T> &X, const T &l) {
    Vector<T> Y = X;
    Y /= l;
    return Y;
  }

  template <typename T> Vector<T> operator+ (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z += Y;
    return Z;
  }

  template <typename T> Vector<T> operator- (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z -= Y;
    return Z;
  }

  template <typename T> std::ostream &operator<< (std::ostream &os, const Vector<T> &V) {
    os << "[";
    for (unsigned int i=0; i<V.size(); ++i) {
      os << V[i];
      if (i < V.size()-1) os << ",";
    }
    os << "]";
    return os;
  }
}

#endif
