/// @file
/// Implementation of the vb::Vector class.

#include <vb/Vector.h>

namespace vb {
  Vector::Vector (unsigned int size, double t) : boost::numeric::ublas::vector<double> (size,t) { };

  void Vector::operator+= (const Vector &O) {
    for (unsigned int i=0; i<this->size(); ++i) (*this)[i] += O[i];
  }

  void Vector::operator-= (const Vector &O) {
    for (unsigned int i=0; i<this->size(); ++i) (*this)[i] -= O[i];
  }

  void Vector::operator*= (double l) {
    for (unsigned int i=0; i<this->size(); ++i) (*this)[i] *= l;
  }

  void Vector::operator/= (double l) {
    for (unsigned int i=0; i<this->size(); ++i) (*this)[i] /= l;
  }

  Vector Vector::operator- () {
    Vector O(this->size());
    for (unsigned int i=0; i<this->size(); ++i) O[i] = -(*this)[i];
    return O;
  }

  double scalar_product (const Vector &X, const Vector &Y) {
    assert (X.size() == Y.size());

    double t = X[0]*Y[0];
    for (unsigned int i=1; i<X.size(); ++i) t += X[i]*Y[i];
    return t;
  }

  Vector operator* (double l, const Vector &X) {
    Vector Y = X;
    Y *= l;
    return Y;
  }

  Vector operator* (const Vector &X, double l) {
    return l*X;
  }

  Vector operator/ (const Vector &X, double l) {
    Vector Y = X;
    Y /= l;
    return Y;
  }

  Vector operator+ (const Vector &X, const Vector &Y) {
    Vector Z = X;
    Z += Y;
    return Z;
  }

  Vector operator- (const Vector &X, const Vector &Y) {
    Vector Z = X;
    Z -= Y;
    return Z;
  }

  std::ostream &operator<< (std::ostream &os, const Vector &V) {
    os << "[";
    for (unsigned int i=0; i<V.size(); ++i) {
      os << V[i];
      if (i < V.size()-1) os << ",";
    }
    os << "]";
    return os;
  }
}
