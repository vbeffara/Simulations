#include <vb/Stream.h>

namespace vb {
	Stream <int> ints () { return Stream <int> ([](Sink<int> & yield) { for (int i=0;;++i) yield(i); }); }
}
