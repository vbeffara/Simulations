
/// @file Vector.h
/// Definitions of a vector template class.

#ifndef __VB_VECTOR_H
#define __VB_VECTOR_H

#include <vector>
#include <iostream>
#include <stdexcept>

namespace vb {

  /// A vector template class.

  template <typename T> class Vector : public std::vector<T> {
    public:
      /** Constructor.
       *
       * @param size The size of the constructed vector.
       * @param t    The value of the entries of the vector.
       */

      Vector (unsigned int size = 0, const T &t = T()) : std::vector<T> (size,t) { };

      /** Add a vector to this one.
       *
       * @param O The vector to add.
       */

      Vector<T> &operator+= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] += O[i];
        return *this;
      }

      /** Subtract a vector from this one.
       *
       * @param O The vector to subtract.
       */

      Vector<T> &operator-= (const Vector<T> &O) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] -= O[i];
        return *this;
      }

      /** Multiply a vector by a scalar.
       *
       * @param l The scalar by which to multiply.
       */

      Vector<T> &operator*= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] *= l;
        return *this;
      }

      /** Divide a vector by a scalar.
       *
       * @param l The scalar by which to divide.
       */

      Vector<T> &operator/= (const T &l) {
        for (unsigned int i=0; i<this->size(); ++i) (*this)[i] /= l;
        return *this;
      }

      /// Compute the opposite of a vector.

      Vector<T> operator- () const {
        Vector O(this->size());
        for (unsigned int i=0; i<this->size(); ++i) O[i] = -(*this)[i];
        return O;
      }
  };

  /** Compute the scalar product of two vectors.
   *
   * Maybe it should be simply written as operator*, but I'm not sure 
   * it's such a good idea ...
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  template <typename T> T scalar_product (const Vector<T> &X, const Vector<T> &Y) {
    if (X.size() != Y.size()) throw std::runtime_error("vb::Vector : wrong dimension.");

    T t = X[0]*Y[0];
    for (unsigned int i=1; i<X.size(); ++i) t += X[i]*Y[i];
    return t;
  }

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  template <typename T> Vector<T> operator* (const T &l, const Vector<T> &X) {
    Vector<T> Y = X;
    Y *= l;
    return Y;
  }

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  template <typename T> Vector<T> operator* (const Vector<T> &X, const T &l) {
    return l*X;
  }

  /** Divide a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  template <typename T> Vector<T> operator/ (const Vector<T> &X, const T &l) {
    Vector<T> Y = X;
    Y /= l;
    return Y;
  }

  /** Compute the sum of two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  template <typename T> Vector<T> operator+ (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z += Y;
    return Z;
  }

  /** Compute the difference between two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  template <typename T> Vector<T> operator- (const Vector<T> &X, const Vector<T> &Y) {
    Vector<T> Z = X;
    Z -= Y;
    return Z;
  }

  /** Output a vector to an output stream.
   *
   * @param os The output stream.
   * @param V  The vector to output.
   */

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

