/// @file
/// Definitions of a matrix class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace vb {
  typedef boost::numeric::ublas::vector<double> Vector;
  typedef boost::numeric::ublas::matrix<double> Matrix;

  inline Matrix operator* (const Matrix &M, const Matrix &N) { return prod(M,N); }
  inline Vector operator* (const Matrix &M, const Vector &X) { return prod(M,X); }
}

#endif
