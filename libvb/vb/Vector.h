
/// @file Vector.h
/// Definitions of a vector template class.

#ifndef __VB_VECTOR_H
#define __VB_VECTOR_H

#include <vector>
#include <iostream>
#include <stdexcept>

namespace vb {
  template <typename T> class Vector : public std::vector<T> {
    public:
      Vector (int size = 0) : std::vector<T> (size) { };
      Vector (int size, const T &t) : std::vector<T> (size,t) { };

      Vector<T> &operator+= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] += O[i];
        return *this;
      }

      Vector<T> &operator-= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] -= O[i];
        return *this;
      }

      Vector<T> &operator*= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] *= l;
        return *this;
      }

      Vector<T> &operator/= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] /= l;
        return *this;
      }

      Vector<T> operator- () const {
        Vector O(this->size());
        for (unsigned int i=0; i<this->size(); ++i) O[i] = -(*this)[i];
        return O;
      }
  };

  template <typename T> T scalar_product (const Vector<T> &X, const Vector<T> &Y) {
    if (X.size() != Y.size()) throw std::runtime_error("vb::Vector : wrong dimension.");

    T t = X[0]*Y[0];
    for (unsigned int i=1; i<X.size(); ++i) t += X[i]*Y[i];
    return t;
  }

  template <typename T> Vector<T> operator* (const T &l, const Vector<T> &X) {
    Vector<T> Y = X;
    Y *= l;
    return Y;
  }

  template <typename T> Vector<T> operator* (const Vector<T> &X, const T &l) {
    return l*X;
  }

  template <typename T> Vector<T> operator/ (const Vector<T> &X, const T &l) {
    Vector<T> Y = X;
    Y /= l;
    return Y;
  }

  template <typename T> Vector<T> operator+ (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z += Y;
    return Z;
  }

  template <typename T> Vector<T> operator- (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z -= Y;
    return Z;
  }

  template <typename T> std::ostream &operator<< (std::ostream &os, const Vector<T> &V) {
    os << "[";
    for (unsigned int i=0; i<V.size(); ++i) {
      os << V[i];
      if (i < V.size()-1) os << ",";
    }
    os << "]";
    return os;
  }
}

#endif
