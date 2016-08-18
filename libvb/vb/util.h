#pragma once
#include <sstream>

namespace vb {
	template <typename T, typename U> T reparse (U z) { std::ostringstream os; os << z; return T (os.str().c_str()); }
}
