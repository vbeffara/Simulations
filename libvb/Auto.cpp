#include <vb/Auto.h>

namespace vb {
	void Auto_update (void * A) { ((Auto*)A) -> update(); }
}