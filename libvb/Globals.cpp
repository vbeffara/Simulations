/// @file
/// Implementation of useful global variables.

#include <vb/PRNG.h>
#include <vb/Auto.h>
#include <vb/coo.h>

namespace vb {
	PRNG prng;
	std::vector<Task> Auto::tasks;

	const coo dz[8] = { coo(1,0), coo(0,1), coo(-1,0), coo(0,-1), coo(1,1), coo(-1,-1), coo(-1,1), coo(1,-1) };
	const coo3 dz3[6] = { coo3(1,0,0), coo3(-1,0,0), coo3(0,1,0), coo3(0,-1,0), coo3(0,0,1), coo3(0,0,-1) };
}
