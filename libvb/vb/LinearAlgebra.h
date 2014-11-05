#pragma once
#define BOOST_UBLAS_TYPE_CHECK 0
#include <boost/numeric/ublas/vector.hpp>
#include <boost/numeric/ublas/vector_proxy.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/triangular.hpp>
#include <boost/numeric/ublas/lu.hpp>
#include <boost/numeric/ublas/io.hpp>

namespace vb {
	namespace ublas = boost::numeric::ublas;

	template <typename T> using Vector = ublas::vector<T>;
	template <typename T> using Matrix = ublas::matrix<T>;

	template <typename T> unsigned my_index_norm_inf (const Vector<T> & v) {
		unsigned out=0; auto m = norm(v(0));
		for (unsigned i=1; i<v.size(); ++i) if (norm(v(i))>m) { m=norm(v(i)); out=i; }
		return out;
	}

	template <typename T, class PM> unsigned my_lu_factorize (Matrix<T> & m, PM &pm) {
	    using M = Matrix<T>;

	    unsigned singular = 0, size1 = m.size1(), size2 = m.size2 (), size = (std::min) (size1, size2);

	    for (unsigned i = 0; i < size; ++ i) {
	        ublas::matrix_column<M> mci (column (m, i));
	        ublas::matrix_row<M> mri (row (m, i));
	        Vector<T> proj = project (mci, ublas::range (i, size1));
	        unsigned tmp = my_index_norm_inf (proj);
	        unsigned i_norm_inf = i + tmp;
	        if (m (i_norm_inf, i) != T(0)) {
	            if (i_norm_inf != i) {
	                pm (i) = i_norm_inf;
	                row (m, i_norm_inf).swap (mri);
	            }
	            T m_inv = T (1) / m (i, i);
	            project (mci, ublas::range (i + 1, size1)) *= m_inv;
	        } else if (singular == 0) {
	            singular = i + 1;
	        }
	        project (m, ublas::range (i + 1, size1), ublas::range (i + 1, size2)).minus_assign (
	            outer_prod (project (mci, ublas::range (i + 1, size1)),
	                        project (mri, ublas::range (i + 1, size2))));
	    }

	    return singular;
	}

	template <typename T> T det (const Matrix<T> & input) {
		Matrix<T> A(input);
		ublas::permutation_matrix<> pivots(A.size1());
		if (my_lu_factorize(A,pivots)) return T(0);

		T det(1);
		for (unsigned i=0; i<pivots.size(); ++i) {
			if (pivots(i)!=i) det *= T(-1);
			det *= A(i,i);
		}
		return det;
	}

	template <typename T> bool inv (const Matrix<T> & input, Matrix<T> & inverse) {
		Matrix<T> A(input);
		ublas::permutation_matrix<> pivots(A.size1());
		if (my_lu_factorize(A,pivots)) return false;

		inverse.assign (ublas::identity_matrix<T>(A.size1()));
		lu_substitute (A,pivots,inverse);
		return true;
	}

	template <typename T> void printmath (std::ostream & os, const Vector<T> & v) {
		os << "{"; bool first=true;
		for (unsigned i=0; i<v.size(); ++i) { os << (first ? " " : ", ") << v(i); first = false; }
		os << "}";
	}

	template <typename T> void printmath (std::ostream & os, const Matrix<T> & A) {
		bool ffirst=true; for (unsigned i=0; i<A.size1(); ++i) {
			os << (ffirst ? "{{" : " },\n{"); ffirst = false;
			bool first=true; for (unsigned j=0; j<A.size2(); ++j) { os << (first ? " " : ", ") << A(i,j); first = false; }
		} os << " }}";
	}
}
