/// @file
/// Definitions of a matrix class.

#ifndef __VB_MATRIX_H
#define __VB_MATRIX_H

#include <vb/common.h>

#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace vb {
  /// A vector of real numbers.
  typedef boost::numeric::ublas::vector<double> Vector;

  /// A matrix of real numbers.
  typedef boost::numeric::ublas::matrix<double> Matrix;
}

#endif
