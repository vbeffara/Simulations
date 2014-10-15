#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
#include <cassert>
#include <iomanip>

using namespace vb; using namespace std;

class Constellation { public:
	cpx lead() {
		Polynomial P ({1}); for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) P.add_root(b[i]);
		cpx avg = 0, avg2 = 0; int d=0;
		for (int i=0; i<w.size(); ++i) { d += wd[i]; avg += P(w[i])*cpx(wd[i]); avg2 += P(w[i])*P(w[i])*cpx(wd[i]); }
		avg /= d; avg2 /= d; return 1.0/avg;
	}

	void compute() {
		Polynomial P ({1}); for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) P.add_root(b[i]); Ps = {P};
		for (auto & c : Ps[0]) c *= l;
		for (int i=0; i<b.size(); ++i) for (int j=Ps.size(); j<bd[i]; ++j) Ps.push_back(Ps.back().derivative());
		for (int i=0; i<w.size(); ++i) for (int j=Ps.size(); j<wd[i]; ++j) Ps.push_back(Ps.back().derivative());
	}

	vector<cpx>     	b,w;
	vector<unsigned>	bd,wd;
	cpx             	l=1.0;

	vector<Polynomial>	Ps;
};

ostream & operator<< (ostream & os, const Constellation & C) {
	os << "Black vertices: " << endl;
	for (int i=0; i<C.b.size(); ++i) {
		os << "| " << C.bd[i] << "\t" << C.b[i];
		for (int j=0; j<C.bd[i]; ++j) os << "\t" << C.Ps[j](C.b[i]);
		os << endl;
	}
	os << endl;
	os << "White vertices: " << endl;
	for (int i=0; i<C.w.size(); ++i) {
		os << "| " << C.wd[i] << "\t" << C.w[i];
		for (int j=0; j<C.wd[i]; ++j) os << "\t" << C.Ps[j](C.w[i]);
		os << endl;
	}
	os << endl;
	os << "P[z] = " << C.Ps[0] << endl;
	return os;
}

double cost (vector<double> xy, Constellation & C) {
	int n1 = C.b.size(), n2 = C.w.size(); assert (xy.size()==2*(n1+n2)+2);

	vector<pair<cpx,unsigned>> black, white;
	for (int i=0; i<n1; ++i) black.push_back ({cpx (xy[2*i],xy[2*i+1]),C.bd[i]});
	for (int i=0; i<n2; ++i) white.push_back ({cpx (xy[2*n1+2*i],xy[2*n1+2*i+1]),C.wd[i]});

	Polynomial P ({1}); for (int i=0; i<n1; ++i) for (int j=0; j<C.bd[i]; ++j) P.add_root(black[i].first);

	for (auto & c : P) c *= cpx (xy[2*n1+2*n2],xy[2*n1+2*n2+1]);

	vector<Polynomial> Ps (1,P); for (int i=0; i<P.degree(); ++i) Ps.push_back(Ps.back().derivative());
	double out=0;
	for (auto z : white) for (int i=0; i<z.second; ++i) out += norm (Ps[i](z.first) - (i==0?1.0:0.0));
	return out;
}

void find (Constellation & C) {
	vector<double> bw;
	for (auto b : C.b) { bw.push_back(real(b)); bw.push_back(imag(b)); }
	for (auto w : C.w) { bw.push_back(real(w)); bw.push_back(imag(w)); }
	bw.push_back(real(C.l)); bw.push_back(imag(C.l));
	double c = cost (bw,C), old_c = c+1, eps = sqrt(c), nc = c;

	while (eps>1e-100) {
		cout << c << " (" << eps << ")          \r";
		old_c = c; bool flag = false;
		for (int i=0; i<bw.size(); ++i) {
			bw[i] += eps; nc = cost (bw,C); if (nc<c) { c=nc; flag=true; } else { bw[i] -= eps; }
			bw[i] -= eps; nc = cost (bw,C); if (nc<c) { c=nc; flag=true; } else { bw[i] += eps; }
		}
		if (!flag) eps /= 2;
	}

	int nb = C.b.size(), nw = C.w.size();
	for (int i=0; i<C.b.size(); ++i) C.b[i] = cpx (bw[2*i],bw[2*i+1]);
	for (int i=0; i<C.w.size(); ++i) C.w[i] = cpx (bw[2*nb+2*i],bw[2*nb+2*i+1]);
	C.l = cpx (bw[2*nb+2*nw],bw[2*nb+2*nw+1]); C.compute();
}

void belyi (Constellation & C) {
	cpx lead = C.Ps[0].back(); double lambda = pow(abs(lead),1.0/C.Ps[0].degree());
	for (int i=1; i<C.Ps[0].size(); ++i) C.Ps[0][i] /= pow(lambda,i);

	for (auto & z : C.b) z *= lambda; for (auto & z : C.w) z *= lambda; C.l = C.lead(); C.compute();

	double err=0;
	for (int k=0; k<C.b.size(); ++k) for (int i=0; i<C.bd[k]; ++i) err += abs(C.Ps[i](C.b[k]));
	for (int k=0; k<C.w.size(); ++k) for (int i=0; i<C.wd[k]; ++i) err += abs(C.Ps[i](C.w[k]) - (i==0?1.0:0.0));

	cout << endl << endl << "Remaining error: " << err << endl;
	cout << setprecision (log10(1/err)-1) << fixed;
	cout << endl << C << endl;
}

Constellation marked (const Spheroidal & T) {
	Constellation C;

	for (int i=0; i<T.sigma.size(); ++i) {
		if (!(T.initial[i]&2)) continue;
		auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
		bool there=false; for (auto w : C.b) if (w==z) there=true; if (there) continue;
		if (!there) { C.b.push_back(z); C.bd.push_back(d); }
	}


	for (int i=0; i<T.sigma.size(); ++i) {
		if (!(T.initial[i]&4)) continue;
		auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
		bool there=false; for (auto w : C.w) if (w==z) there=true; if (there) continue;
		if (!there) { C.w.push_back(z); C.wd.push_back(d); }
	}

	C.l = C.lead(); C.compute(); return C;
}

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} };                                	// Graph tripod
	// Hypermap M { {{0,1,2}}, {{0},{1},{2}}, {{2,1,0}} };                                                        	// Dessin tripod
	// Hypermap M { {{0,1,2,3}}, {{0},{1},{2},{3}}, {{3,2,1,0}} };                                                	// Dessin 4-star
	// Hypermap M { {{0,1,2,3,4}}, {{0},{1},{2},{3},{4}}, {{4,3,2,1,0}} };                                        	// Dessin 5-star
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} };                	// Long graph tripod
	Hypermap M { {{2,1,0},{3}}, {{2},{1},{0,3}}, {{2,0,3,1}} };                                                   	// Long dessin tripod - Lando page 87
	// Hypermap M { {{0,1,2},{3,4},{5,6}}, {{0},{1,3},{2,5},{4},{6}}, {{0,2,6,5,1,4,3}} };                        	// Long-legged tripod - Lando page 88 - draw BUG
	// Hypermap M { {{4,1,2},{3,0},{5,6}}, {{4,3},{1},{2},{0,5},{6}}, {{4,0,6,5,3,2,1}} };                        	// Long-tailed tripod - Lando page 89
	// Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} };	// Graph H
	// Hypermap M { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} };                                        	// Dessin H

	cout << M;
	cout << "  sigma: " << M.sigma << endl;
	cout << "  alpha: " << M.alpha << endl;
	cout << "    phi: " << M.phi << endl;

	if (!H['d']) M.dessin();

	for (int i=0; i<int(H['n']); ++i) M.split_edges(); Spheroidal T (M,H); T.pack(); cout << "               \r";

	unsigned i=0; while (!(T.initial[i]&8)) ++i; auto & v = T.V[T.E[i].src]; T.linear (1,-v.z); T.inversion(); T.linear (-1/v.r,0); T.output_pdf();
	{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); } T.linear (std::polar(1.0,-T.E[0].a)); T.output_pdf();

	Constellation C = marked(T);
	cout << endl << C << endl;

	cpx lead = C.Ps[0].back(); double lambda = pow(abs(lead),1.0/C.Ps[0].degree());
	for (auto & z : C.b) z *= lambda; for (auto & z : C.w) z *= lambda; C.l = C.lead(); C.compute();

	find(C);

	cpx z0=C.b[0]; for (auto & z : C.b) z -= z0; for (auto & z : C.w) z -= z0;
	cpx r = (C.b.size()>1 ? polar(1.0,-arg(C.b[1])) : polar(1.0,-arg(C.w[0]))); for (auto & z : C.b) z *= r; for (auto & z : C.w) z *= r;

	belyi(C);
}
