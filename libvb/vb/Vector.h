/// @file
/// Definitions of a vector class.

#ifndef __VB_VECTOR_H
#define __VB_VECTOR_H

#include <vb/common.h>
#include <boost/numeric/ublas/vector.hpp>

namespace vb {

  /// A vector class.

  class Vector : public boost::numeric::ublas::vector<double> {
    public:
      /** Constructor.
       *
       * @param size The size of the constructed vector.
       * @param t    The value of the entries of the vector.
       */

      Vector (unsigned int size = 0, double t = 0);

      /** Add a vector to this one.
       *
       * @param O The vector to add.
       */

      void operator+= (const Vector &O);

      /** Subtract a vector from this one.
       *
       * @param O The vector to subtract.
       */

      void operator-= (const Vector &O);

      /** Multiply a vector by a scalar.
       *
       * @param l The scalar by which to multiply.
       */

      void operator*= (double l);

      /** Divide a vector by a scalar.
       *
       * @param l The scalar by which to divide.
       */

      void operator/= (double l);

      /// Compute the opposite of a vector.

      Vector operator- ();
  };

  /** Compute the scalar product of two vectors.
   *
   * Maybe it should be simply written as operator*, but I'm not sure 
   * it's such a good idea ...
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  double scalar_product (const Vector &X, const Vector &Y);

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator* (double l, const Vector &X);

  /** Multiply a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator* (const Vector &X, double l);

  /** Divide a vector by a scalar.
   *
   * @param X The vector.
   * @param l The scalar.
   */

  Vector operator/ (const Vector &X, double l);

  /** Compute the sum of two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  Vector operator+ (const Vector &X, const Vector &Y);

  /** Compute the difference between two vectors.
   *
   * @param X The first vector.
   * @param Y The second vector.
   */

  Vector operator- (const Vector &X, const Vector &Y);

  /** Output a vector to an output stream.
   *
   * @param os The output stream.
   * @param V  The vector to output.
   */

  std::ostream &operator<< (std::ostream &os, const Vector &V);
}

#endif

