/// @file
/// Implementation of the global state variables.

#include <vb/PRNG.h>
#include <vb/Auto.h>

namespace vb {
	PRNG prng;
	std::vector<Task> Auto::tasks;
}
