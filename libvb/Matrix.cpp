/// @file
/// Implementation os the vb::Matrix class and helpers.

#include <vb/Matrix.h>

namespace vb {
  Matrix::Matrix (unsigned int h, unsigned int w)
    : boost::numeric::ublas::matrix<double>(h,w), width(w), height(h) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : boost::numeric::ublas::matrix<double>(h,w), width(w), height(h) {
      for (int i=0; i<d.size(); ++i) (*this)(i,i) = d[i];
    }

  Matrix & Matrix::operator*= (const Matrix &M) {
    (boost::numeric::ublas::matrix<double>)((*this)) = prod (*this, M);
    return *this;
  }

  Matrix & Matrix::rank1update (const Vector &A, const Vector &B) {
    (boost::numeric::ublas::matrix<double>)(*this) += outer_prod(A,B);
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
    return prod (M, X);
  }
}
