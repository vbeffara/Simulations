#pragma once
#define BOOST_UBLAS_TYPE_CHECK 0
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>

namespace vb {
	namespace ublas = boost::numeric::ublas;

	template <typename T> using Vector = ublas::vector<T>;
	template <typename T> using Matrix = ublas::matrix<T>;

	template <typename T> T det (const Matrix<T> & input);

	template <typename T> bool inv (const Matrix<T> & input, Matrix<T> & inverse);

	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y);
}

#include <vb/impl/LinearAlgebra.hxx>
