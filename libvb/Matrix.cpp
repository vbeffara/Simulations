#include <vb/Matrix.h>

namespace vb {
  Matrix::Matrix (unsigned int h, unsigned int w)
    : width(w), height(h), data (new MatrixStorage_DiagSmallRank (h,w)) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : width(w), height(h), data (new MatrixStorage_DiagSmallRank (h,w,d)) {}

  Matrix::Matrix (const Matrix &M)
    : width(M.width), height(M.height), data(M.data->copy()) {}

  Matrix::~Matrix () {
    delete data;
  }

  Matrix & Matrix::operator= (const Matrix &M) {
    if (&M != this) {
      width = M.width; height = M.height;
      delete data; data = M.data->copy();
    }
    return (*this);
  }

  double Matrix::at (unsigned int i, unsigned int j) const { return data->at(i,j); }

  double Matrix::operator() (unsigned int i, unsigned int j) const { return this->at(i,j); }

  Matrix & Matrix::put (unsigned int i, unsigned int j, double t) { 
    MatrixStorage * tmp = data->put(i,j,t);
    if (data != tmp) { delete data; data = tmp; }
    return (*this);
  }

  Matrix & Matrix::operator+= (const Matrix &M) {
    MatrixStorage *tmp = data->add (M.data);
    if (data != tmp) { delete data; data = tmp; }
    return (*this);
  }

  Matrix & Matrix::operator-= (const Matrix &M) {
    MatrixStorage *tmp = data->sub(M.data);
    if (data != tmp) { delete data; data = tmp; }
    return (*this);
  }

  Matrix & Matrix::operator*= (const Matrix &M) {
    MatrixStorage *tmp = data->mul_right (M.data);
    if (data != tmp) { delete data; data = tmp; }
    return (*this);
  }

  Matrix & Matrix::rank1update (const Vector &A, const Vector &B) {
    MatrixStorage *tmp = data->rank1update(A,B);
    if (data != tmp) { delete data; data = tmp; }
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
    return M.data->map_right(X);
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
