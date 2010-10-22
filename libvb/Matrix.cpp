/// @file
/// Implementation os the vb::Matrix class and helpers.

#include <vb/Matrix.h>

namespace vb {
  MatrixStorage_Plain::MatrixStorage_Plain (unsigned int h, unsigned int w) :
    boost::numeric::ublas::matrix<double> (h,w), width(w), height(h) { }

  void MatrixStorage_Plain::put (unsigned int i, unsigned int j, double t) {
    (*this)(i,j) = t;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::add (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<height; ++i)
      for (unsigned int j=0; j<width; ++j)
        (*this)(i,j) += (*M)(i,j);
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::sub (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<height; ++i)
      for (unsigned int j=0; j<width; ++j)
        (*this)(i,j) -= (*M)(i,j);
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::mul_right (MatrixStorage_Plain *M) {
    MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, M->width);
    for (unsigned int i=0; i<height; ++i) {
      for (unsigned int j=0; j<M->width; ++j) {
        (*tmp)(i,j) = (*this)(i,0) * (*M)(0,j);
        for (unsigned int k=1; k<width; ++k) 
          (*tmp)(i,j) += (*this)(i,k) * (*M)(k,j);
      }
    }
    return tmp;
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
    : width(w), height(h), data (new MatrixStorage_Plain (h,w)) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : width(w), height(h), data (new MatrixStorage_Plain (h,w)) {
      for (int i=0; i<d.size(); ++i) data -> put (i,i,d[i]);
    }

  Matrix::Matrix (const Matrix &M) : width(M.width), height(M.height) {
    for (int i=0; i<height; ++i)
      for (int j=0; j<width; ++j)
        (*data)(i,j) = (*M.data)(i,j);
  }

  Matrix::~Matrix () {
    delete data;
  }

  Matrix & Matrix::operator= (const Matrix &M) {
    if (&M != this) {
      width = M.width; height = M.height;
      delete data; data = new MatrixStorage_Plain (M.height, M.width);
      for (int i=0; i<height; ++i)
        for (int j=0; j<width; ++j)
          (*data)(i,j) = (*M.data)(i,j);
    }
    return (*this);
  }

  double Matrix::operator() (unsigned int i, unsigned int j) const { return (*data)(i,j); }

  void Matrix::put (unsigned int i, unsigned int j, double t) { 
    data->put(i,j,t);
  }

  Matrix & Matrix::operator+= (const Matrix &M) {
    data->add (M.data);
    return (*this);
  }

  Matrix & Matrix::operator-= (const Matrix &M) {
    data->sub(M.data);
    return (*this);
  }

  Matrix & Matrix::operator*= (const Matrix &M) {
    MatrixStorage_Plain *tmp = data->mul_right (M.data);
    if (data != tmp) { delete data; data = tmp; }
    return (*this);
  }

  Matrix & Matrix::rank1update (const Vector &A, const Vector &B) {
    MatrixStorage_Plain *tmp = data->rank1update(A,B);
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
