#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Spheroidal.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} }; // Tripod
	// Hypermap M { {{1},{5},{7},{3,4},{0,6,2}}, {{0,1},{2,3},{4,5},{6,7}}, {{0,1,2,4,5,3,6,7}} }; // Long tripod
	Hypermap M { {{1},{5},{7},{9},{0,8,2},{4,3,6}}, {{0,1},{2,3},{4,5},{6,7},{8,9}}, {{0,1,2,4,5,6,7,3,8,9}} }; // H shape

	cerr << "sigma:   " << M.sigma.passport() << endl;
	cerr << "alpha:   " << M.alpha.passport() << endl;
	cerr << "phi:     " << M.phi.passport()   << endl;
	cerr << "genus:   " << M.genus()   << endl;

	cerr << endl;

	if (!H['d']) M.dessin();
	for (int i=0; i<int(H['n']); ++i) M.split_edges();
	M.simplify();

	Spheroidal T (M,H); T.pack();

	unsigned i=0; while (!(T.initial[i]&8)) ++i; cpx z = T.V[T.E[i].src].z;

	T.linear (1,-z); T.inversion();
	double mr = fabs(T.V[0].r); for (auto v : T.V) mr = std::max (mr, fabs(v.r)); T.linear (1/mr,0);

	cerr << "sigma:   " << M.sigma.passport() << endl;
	cerr << "alpha:   " << M.alpha.passport() << endl;
	cerr << "phi:     " << M.phi.passport()   << endl;

	T.output_pdf();
}