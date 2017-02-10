#pragma once
#include <vb/LinearAlgebra.h>

namespace vb {
	template <typename T> Vector<T> solve (const Matrix<T> & A, const Vector<T> & y) {
		return A.colPivHouseholderQr().solve(y);
	}
}
