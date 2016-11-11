#include <vb/Hub.h>
#include <vb/Stream_lib.h>
#include <algorithm>
#include <iomanip>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	H.init ("Streams and permutations", argc, argv, "n=10");
	for (unsigned k=1; k<=unsigned(H['n']); ++k) {
		cout << "n: "             	<< setw(2) << k;
		cout << "\tpermutations: "	<< setw(7) << size(permutations(k));
		cout << "\tpartitions: "  	<< setw(2) << size(partitions(k)) << endl;
	}
}
