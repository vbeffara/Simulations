#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

int main (int argc, char **argv) {
	H.init ("Rarefaction fan", argc, argv, "n=20");
	int n = H['n'];

	vector<int> field (2*n); for (int i=0; i<2*n; i++) field[i]=n-i;

	while (true) {
		int i = prng.uniform_int(2*n-1);
		if (field[i]>field[i+1]) swap (field[i],field[i+1]);
		if ((field[0]<0) || (field[2*n-1]>0)) break;
	}

	for (int i=0; i<2*n; i++) cout << i << " " << field[i] << endl;
}
