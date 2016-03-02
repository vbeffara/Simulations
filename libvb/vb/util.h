#pragma once

namespace vb {
	template <typename T, typename U> T reparse (U z) { ostringstream os; os << z; return T (os.str().c_str()); }
}
