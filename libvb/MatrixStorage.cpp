/// @file
/// Implementation of the MatrixStorage helper class.

#include <vb/MatrixStorage.h>

namespace vb {
  MatrixStorage_Plain::MatrixStorage_Plain (unsigned int h, unsigned int w) :
    width(w), height(h), lines (std::vector<Vector> (h,Vector(w))) { }

  MatrixStorage_Plain * MatrixStorage_Plain::copy () {
    return new MatrixStorage_Plain (*this);
  }

  MatrixStorage_Plain * MatrixStorage_Plain::compute () {
    return this;
  }

  double  MatrixStorage_Plain::at (unsigned int i, unsigned int j) const {
    return lines[i][j];
  }

  MatrixStorage_Plain * MatrixStorage_Plain::put (unsigned int i, unsigned int j, double t) {
    lines[i][j] = t; return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::add (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] += M->at(i,j);
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::sub (MatrixStorage_Plain *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] -= M->at(i,j);
    return this;
  }

  MatrixStorage_Plain * MatrixStorage_Plain::mul_right (MatrixStorage_Plain *M) {
    MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, M->width);
    for (unsigned int i=0; i<this->height; ++i) {
      for (unsigned int j=0; j<M->width; ++j) {
        tmp->lines[i][j] = lines[i][0] * M->at(0,j);
        for (unsigned int k=1; k<this->width; ++k) 
          tmp->lines[i][j] += lines[i][k] * M->at(k,j);
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
}
