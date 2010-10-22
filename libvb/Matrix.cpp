/// @file
/// Implementation os the vb::Matrix class and helpers.

#include <vb/Matrix.h>

namespace vb {
  MatrixStorage_Plain::MatrixStorage_Plain (unsigned int h, unsigned int w) :
    boost::numeric::ublas::matrix<double> (h,w), width(w), height(h) { }

  void MatrixStorage_Plain::put (unsigned int i, unsigned int j, double t) {
    (*this)(i,j) = t;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::rank1update (const Vector &A, const Vector &B) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        (*this)(i,j) += A[i]*B[j];
    return this;
  }

  Vector MatrixStorage_Plain::map_right (const Vector &X) {
    return prod (*this, X);
  }

  /******************************************/

  Matrix::Matrix (unsigned int h, unsigned int w)
    : width(w), height(h), data (h,w) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : width(w), height(h), data (h,w) {
      for (int i=0; i<d.size(); ++i) data.put (i,i,d[i]);
    }

  Matrix::Matrix (const Matrix &M) : width(M.width), height(M.height),
    data (M.data) { }

  Matrix::~Matrix () {
  }

  Matrix & Matrix::operator= (const Matrix &M) {
    if (&M != this) {
      width = M.width; height = M.height;
      data = M.data;
    }
    return (*this);
  }

  double Matrix::operator() (unsigned int i, unsigned int j) const { return data(i,j); }

  void Matrix::put (unsigned int i, unsigned int j, double t) { 
    data.put(i,j,t);
  }

  Matrix & Matrix::operator+= (const Matrix &M) {
    data += M.data;
    return (*this);
  }

  Matrix & Matrix::operator-= (const Matrix &M) {
    data -= M.data;
    return (*this);
  }

  Matrix & Matrix::operator*= (const Matrix &M) {
    (boost::numeric::ublas::matrix<double>)(data) = prod (data, M.data);
    return *this;
  }

  Matrix & Matrix::rank1update (const Vector &A, const Vector &B) {
    data.rank1update(A,B);
    return (*this);
  }

  Matrix operator+ (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O+=N;
    return O;
  }

  Matrix operator- (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O-=N;
    return O;
  }

  Matrix operator* (const Matrix &M, const Matrix &N) {
    Matrix O=M;
    O*=N;
    return O;
  }

  Vector operator* (const Matrix &M, const Vector &X) {
    return M.data.map_right(X);
  }

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
