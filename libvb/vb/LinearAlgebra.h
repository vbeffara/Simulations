#pragma once
#define BOOST_UBLAS_TYPE_CHECK 0
#include <vb/cpx.h>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <Eigen/Dense>

namespace vb {
	namespace ublas = boost::numeric::ublas;

	template <typename T> using Vector = Eigen::Matrix <T,Eigen::Dynamic,1>;
	template <typename T> using Matrix = Eigen::Matrix <T,Eigen::Dynamic,Eigen::Dynamic>;

	template <typename T> T det (const Matrix<T> & input);

	template <typename T> bool inv (const Matrix<T> & input, Matrix<T> & inverse);

	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y);
}

#include <vb/impl/LinearAlgebra.hxx>
