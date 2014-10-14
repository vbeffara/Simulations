#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
#include <iomanip>

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

vector<pair<cpx,unsigned>> ones (const Spheroidal & T) {
	vector<pair<cpx,unsigned>> out;

	for (int i=0; i<T.sigma.size(); ++i) {
		unsigned label = T.initial[i]; if (!(label&4)) continue;
		auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
		bool there=false; for (auto w : out) if (w.first==z) there=true; if (there) continue;
		if (!there) out.push_back({z,d});
	}

	return out;
}

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} };                                	// Graph tripod
	// Hypermap M { {{0,1,2}}, {{0},{1},{2}}, {{2,1,0}} };                                                        	// Dessin tripod
	// Hypermap M { {{0,1,2,3}}, {{0},{1},{2},{3}}, {{3,2,1,0}} };                                                	// Dessin 4-star
	// Hypermap M { {{0,1,2,3,4}}, {{0},{1},{2},{3},{4}}, {{4,3,2,1,0}} };                                        	// Dessin 5-star
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} };                	// Long graph tripod
	Hypermap M { {{2,1,0},{3}}, {{2},{1},{0,3}}, {{2,0,3,1}} };                                                   	// Long dessin tripod - Lando page 87
	// Hypermap M { {{0,1,2},{3,4},{5,6}}, {{0},{1,3},{2,5},{4},{6}}, {{0,2,6,5,1,4,3}} };                        	// Long-legged tripod - Lando page 88
	// Hypermap M { {{4,1,2},{3,0},{5,6}}, {{4,3},{1},{2},{0,5},{6}}, {{4,0,6,5,3,2,1}} };                        	// Long-tailed tripod - Lando page 89
	// Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} };	// Graph H
	// Hypermap M { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} };                                        	// Dessin H - BUG !!!

	cout << M;
	cout << "  sigma: " << M.sigma << endl;
	cout << "  alpha: " << M.alpha << endl;
	cout << "    phi: " << M.phi << endl;

	if (!H['d']) M.dessin();

	for (int i=0; i<int(H['n']); ++i) M.split_edges(); Spheroidal T (M,H); T.pack(); cout << "               \r";

	unsigned i=0; while (!(T.initial[i]&8)) ++i; auto & v = T.V[T.E[i].src];
	T.linear (1,-v.z); T.inversion(); T.linear (-1/v.r,0); T.output_pdf();

	{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); }
	T.linear (std::polar(1.0,-T.E[0].a)); T.output_pdf();

	cout << setprecision(10) << fixed;

	Polynomial P ({1});
	for (auto z : roots(T)) for (int i=0; i<z.second; ++i) P.add_root(z.first);

	cpx avg = 0, avg2 = 0; int d=0;
	for (auto z : ones(T)) { d += z.second; avg += P(z.first)*cpx(z.second); avg2 += P(z.first)*P(z.first)*cpx(z.second); }
	avg /= d; avg2 /= d; for (auto & c : P) c /= avg;

	cpx lead = P.back(); double lambda = pow(abs(lead),1.0/(P.size()-1));
	for (int i=1; i<P.size(); ++i) P[i] /= pow(lambda,i);
	T.linear(lambda); T.output_pdf();

	double eps = sqrt(abs(avg2-avg*avg)/norm(avg));
	for (auto & c : P) {
		if (fabs(real(c))<eps) c = cpx(0,imag(c));
		if (fabs(imag(c))<eps) c = cpx(real(c),0);
	}

	cout << setprecision (log10(1/eps)+2);
	cout << endl << "Expected precision: " << eps << endl;
	cout << endl << P << endl;
	cout << endl << "Black vertices:" << endl; for (auto z : roots(T)) cout << "  " << z.second << "\t" << z.first << "\t" << P(z.first) << endl;
	cout << endl << "White vertices:" << endl; for (auto z : ones(T)) cout << "  " << z.second << "\t" << z.first << "\t" << P(z.first) << endl;
}
