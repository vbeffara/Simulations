#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Spheroidal.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	Hypermap M = H_T();

	cerr << "sigma:   " << M.sigma.passport() << endl;
	cerr << "alpha:   " << M.alpha.passport() << endl;
	cerr << "phi:     " << M.phi.passport()   << endl;
	cerr << "genus:   " << M.genus()   << endl;

	cerr << endl;

	if (!H['d']) M.dessin();
	for (int i=0; i<int(H['n']); ++i) M.split_edges();

	Spheroidal T (M,H); T.pack();

	unsigned i=0; while (!(T.initial[i]&8)) ++i; cpx z = T.V[T.E[i].src].z;

	T.linear (1,-z); T.inversion();

	cerr << "sigma:   " << M.sigma.passport() << endl;
	cerr << "alpha:   " << M.alpha.passport() << endl;
	cerr << "phi:     " << M.phi.passport()   << endl;

	T.output_pdf();
}