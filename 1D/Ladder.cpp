
/*
 * env[i] = nb of unvisited edges on top of [i,i+1]
 *        = 2 to the right of max(range)==env.size()
 */

#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
	H.init ("Ladder", argc,argv, "t=20,d");
	int t=H['t'];
	bool d=H['d'];

	vector<int> env;
	int X=0;

	for (int i=0; i<t; ++i) {
		if (d) {
			for (int j=0; j<env.size(); ++j) {
				if (X==j) cout << "*"; else cout << " ";
				cout << env[j];
			}
			if (X==env.size()) cout << "*"; else cout << " ";
			cout << "2 2 2 ..." << endl;
		} else cout << X << endl;

		double p;
		if (X==0) p = 1;
		else p = (env[X-1]==1 ? 2.0/3.0 : 1.0/2.0);

		int dX = 2*prng.bernoulli(p) - 1;

		if ((X==env.size()) && (dX==1)) env.push_back(2);
		if ((dX==1) && (env[X]==1) && prng.bernoulli(.5)) env[X]=0;
		if ((dX==1) && (env[X]==2)) env[X]=1;
		X += dX;
	}
}
