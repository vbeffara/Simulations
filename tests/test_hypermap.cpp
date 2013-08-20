#include <vb/Hypermap.h>
#include <iostream>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {

	// This is the planar hypermap in Zvonkine's paper. Encode using cycles.
	Hypermap H1;
	H1.sigma	= { { 0, 1, 2 }, { 3, 4 }, { 5 }, { 6, 7, 8 } };
	H1.alpha	= { { 0, 3 }, { 1, 8, 2 }, { 4, 5, 6 }, { 7 } };
	H1.phi  	= { { 0, 4, 8 }, { 1 }, { 2, 7, 6, 5, 3 } };

	H1.validate(); cerr << H1;

	// This is a toric graph whose lift is the centered square lattice. Encode using images.
	Hypermap H2;
	H2.sigma	= { 1, 2, 3, 4, 5, 6, 7, 0, 11, 8, 9, 10 };
	H2.alpha	= { 4, 11, 6, 10, 0, 9, 2, 8, 7, 5, 3, 1 };
	H2.phi  	= { 3, 8, 5, 11, 7, 10, 1, 9, 6, 4, 2, 0 };

	H2.validate(); cerr << H2;

	H1.output_dot(cout);

	return 0;
}
