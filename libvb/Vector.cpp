/// @file
/// Implementation of the vb::Vector class.

#include <vb/Vector.h>

namespace vb {
  Vector::Vector (unsigned int size, double t) : boost::numeric::ublas::vector<double> (size,t) { };

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
