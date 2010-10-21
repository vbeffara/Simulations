/// @file
/// Implementation os the vb::Matrix class and helpers.

#include <vb/Matrix.h>

namespace vb {
  MatrixStorage_Plain::MatrixStorage_Plain (unsigned int h, unsigned int w) :
    width(w), height(h), lines (std::vector<Vector> (h,Vector(w))) { }

  MatrixStorage_Plain * MatrixStorage_Plain::copy () {
    return new MatrixStorage_Plain (*this);
  }

  MatrixStorage_Plain * MatrixStorage_Plain::put (unsigned int i, unsigned int j, double t) {
    lines[i][j] = t; return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::add (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] += (M->lines)[i][j];
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::sub (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] -= (M->lines)[i][j];
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::mul_right (MatrixStorage_Plain *M) {
    MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, M->width);
    for (unsigned int i=0; i<this->height; ++i) {
      for (unsigned int j=0; j<M->width; ++j) {
        tmp->lines[i][j] = lines[i][0] * (M->lines)[0][j];
        for (unsigned int k=1; k<this->width; ++k) 
          tmp->lines[i][j] += lines[i][k] * (M->lines)[k][j];
      }
    }
    return tmp;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::rank1update (const Vector &A, const Vector &B) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] += A[i]*B[j];
    return this;
  }

  Vector MatrixStorage_Plain::map_right (const Vector &X) {
    Vector Y(this->height);
    for (unsigned int i=0; i<this->height; ++i)
      Y[i] = inner_prod (lines[i],X);
    return Y;
  }

  /******************************************/

  Matrix::Matrix (unsigned int h, unsigned int w)
    : width(w), height(h), data (new MatrixStorage_Plain (h,w)) {}

  Matrix::Matrix (unsigned int h, unsigned int w, const Vector &d)
    : width(w), height(h), data (new MatrixStorage_Plain (h,w)) {
      for (int i=0; i<d.size(); ++i) data -> put (i,i,d[i]);
    }

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

  double Matrix::operator() (unsigned int i, unsigned int j) const { return data->lines[i][j]; }

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
