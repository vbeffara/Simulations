
/*
 * env[i] = nb of cookies left at site i
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"t=20,p=.67");
	int t=CLP('t');
	double p=CLP('p');

	vector<int> env;
	int X=0; env.push_back(1);

	for (int i=0; i<t; ++i) {
		cout << X << endl;

		int dX = 2*prng.bernoulli(env[X]>0 ? p : .5) - 1;
		if (env[X]>0) env[X]--;

		X += dX; if (X==-1) X=1;
		
		if (X==env.size()) env.push_back(1);
	}
}