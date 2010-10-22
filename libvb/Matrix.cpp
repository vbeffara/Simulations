/// @file
/// Implementation os the vb::Matrix class and helpers.

#include <vb/Matrix.h>

namespace vb {
  Matrix::Matrix (unsigned int h, unsigned int w)
    : width(w), height(h), data (h,w) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : width(w), height(h), data (h,w) {
      for (int i=0; i<d.size(); ++i) data(i,i) = d[i];
    }

  double Matrix::operator() (unsigned int i, unsigned int j) const { return data(i,j); }

  void Matrix::put (unsigned int i, unsigned int j, double t) { 
    data(i,j) = t;
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
    data += outer_prod(A,B);
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
    return prod (M.data, X);
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
