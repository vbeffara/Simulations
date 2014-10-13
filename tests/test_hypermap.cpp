#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Spheroidal.h>

using namespace vb; using namespace std;

vector<pair<cpx,unsigned>> roots (const Spheroidal & T) {
	vector<pair<cpx,unsigned>> out;

	for (int i=0; i<T.sigma.size(); ++i) {
		unsigned label = T.initial[i]; if (!(label&2)) continue;
		auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
		bool there=false; for (auto w : out) if (w.first==z) there=true; if (there) continue;
		if (!there) out.push_back({z,d});
	}

	return out;
}

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} }; // Tripod
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} }; // Long tripod
	Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} }; // H shape

	if (!H['d']) M.dessin();
	M.relabel (Transposition(M.sigma.size(),0,M.alpha[0]));

	for (int i=0; i<int(H['n']); ++i) M.split_edges(); Spheroidal T (M,H); T.pack();

	unsigned i=0; while (!(T.initial[i]&8)) ++i; auto & v = T.V[T.E[i].src];
	T.linear (1,-v.z); T.inversion(); T.linear (-1/v.r,0); T.output_pdf();

	{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); }
	T.linear (std::polar(1.0,-T.E[0].a));
	T.output_pdf();

	cerr << endl << endl;
	for (auto zd : roots(T)) cerr << zd.second << " " << zd.first << endl;

	cerr << endl;
	for (auto z : roots(T)) cerr << "(z - (" << real(z.first) << " + " << imag(z.first) << " I))^" << z.second << " ";
	cerr << endl;
}
