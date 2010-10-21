/// @file
/// Implementation of the MatrixStorage helper class.

#include <vb/MatrixStorage.h>

namespace vb {
  MatrixStorage::MatrixStorage (unsigned int h, unsigned int w) :
    width(w), height(h) { }

  /**************************************************/

  MatrixStorage_Plain::MatrixStorage_Plain (unsigned int h, unsigned int w) :
    MatrixStorage (h,w), lines (std::vector<Vector> (h,Vector(w))) { }

  MatrixStorage * MatrixStorage_Plain::copy () {
    return new MatrixStorage_Plain (*this);
  }

  MatrixStorage * MatrixStorage_Plain::compute () {
    return this;
  }

  double  MatrixStorage_Plain::at (unsigned int i, unsigned int j) const {
    return lines[i][j];
  }

  MatrixStorage * MatrixStorage_Plain::put (unsigned int i, unsigned int j, double t) {
    lines[i][j] = t; return this;
  }

  MatrixStorage * MatrixStorage_Plain::add (MatrixStorage *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] += M->at(i,j);
    return this;
  }

  MatrixStorage * MatrixStorage_Plain::sub (MatrixStorage *M) {
    for (unsigned int i=0; i<this->height; ++i)
      for (unsigned int j=0; j<this->width; ++j)
        lines[i][j] -= M->at(i,j);
    return this;
  }

  MatrixStorage * MatrixStorage_Plain::mul_right (MatrixStorage *M) {
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

  MatrixStorage * MatrixStorage_Plain::rank1update (const Vector &A, const Vector &B) {
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

  /**************************************************/

  MatrixStorage_DiagSmallRank::MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w)
    : MatrixStorage (h,w), diag (Vector(h,0)) { }

  MatrixStorage_DiagSmallRank::MatrixStorage_DiagSmallRank (unsigned int h, unsigned int w, const Vector &d)
    : MatrixStorage (h,w), diag(d) { }

  MatrixStorage * MatrixStorage_DiagSmallRank::copy () {
    return new MatrixStorage_DiagSmallRank (*this);
  }

  MatrixStorage *MatrixStorage_DiagSmallRank::compute () {
    MatrixStorage_Plain *tmp = new MatrixStorage_Plain (this->height, this->width);
    for (unsigned int i=0; i<this->height; ++i) {
      for (unsigned int j=0; j<updates.size(); ++j)
        tmp->lines[i] += updates[j].first[i] * updates[j].second;
    }
    for (unsigned int i=0; i<(this->height<this->width?this->height:this->width); ++i)
      tmp->lines[i][i] += diag[i];
    return tmp;
  }

  double MatrixStorage_DiagSmallRank::at (unsigned int i, unsigned int j) const {
    double tmp = 0;
    if (i==j) tmp += diag[i];
    for (unsigned int k=0; k<updates.size(); ++k)
      tmp += updates[k].first[i] * updates[k].second[j];
    return tmp;
  }

  MatrixStorage * MatrixStorage_DiagSmallRank::put (unsigned int i, unsigned int j, double t) {
    MatrixStorage *tmp = this->compute();
    tmp->put (i,j,t);
    return tmp;
  }

  MatrixStorage * MatrixStorage_DiagSmallRank::add (MatrixStorage *M) {
    MatrixStorage *tmp = this->compute();
    tmp->add(M);
    return tmp;
  }

  MatrixStorage * MatrixStorage_DiagSmallRank::sub (MatrixStorage *M) {
    MatrixStorage *tmp = this->compute();
    tmp->sub(M);
    return tmp;
  }

  MatrixStorage * MatrixStorage_DiagSmallRank::mul_right (MatrixStorage *M) {
    MatrixStorage *tmp = this->compute();
    tmp->mul_right(M);
    return tmp;
  }

  MatrixStorage * MatrixStorage_DiagSmallRank::rank1update (const Vector &A, const Vector &B) {
    updates.push_back (std::pair<Vector,Vector> (A,B));
    if (updates.size() > this->height/10) {
      return this->compute();
    } else {
      return this;
    }
  }

  Vector MatrixStorage_DiagSmallRank::map_right (const Vector &X) {
    Vector tmp(this->width);
    for (unsigned int i=0; i<(this->width<this->height?this->width:this->height); ++i) tmp[i] = diag[i]*X[i];
    for (unsigned int i=0; i<updates.size(); ++i) tmp += updates[i].first * inner_prod(updates[i].second,X);
    return tmp;
  }
}
