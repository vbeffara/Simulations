#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
#include <cassert>
#include <iomanip>

using namespace vb; using namespace std;

class RationalFraction { public:
	RationalFraction (Polynomial P_ = {{1}}, Polynomial Q_ = {{1}}) : P(P_), Q(Q_) {}

	cpx operator() (cpx z) const { return P(z)/Q(z); }

	int degree () const { return P.degree() - Q.degree(); }

	void add_root (cpx z) { P.add_root(z); }
	void add_pole (cpx z) { Q.add_root(z); }

	RationalFraction derivative () const { const Polynomial PP=P.derivative(), QQ=Q.derivative(); return {Q*PP-P*QQ,Q*Q}; }

	Polynomial P,Q;
};

ostream & operator<< (ostream & os, const RationalFraction & R) {
	os << "P[z_] := " << R.P << endl;
	os << "Q[z_] := " << R.Q << endl;
	return os;
}

class Constellation { public:
	Constellation (const Spheroidal & T) {
		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&2)) continue;
			auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : b) if (zz==z) there=true;
			if (!there) { b.push_back(z); bd.push_back(d); }
		}

		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&4)) continue;
			auto & v = T.V[T.E[i].src]; cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : w) if (zz==z) there=true;
			if (!there) { w.push_back(z); wd.push_back(d); }
		}

		for (int i=0; i<T.sigma.size(); ++i) {
			if (!(T.initial[i]&8)) continue;
			auto & v = T.V[T.E[i].src]; if (v.r<0) continue;
			cpx z = v.z; unsigned d = v.adj.size()/2;
			bool there=false; for (auto zz : f) if (zz==z) there=true;
			if (!there) { f.push_back(z); fd.push_back(d); }
		}

		compute();
	}

	void compute () {
		RationalFraction R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		Rs = {R};
		for (auto & c : Rs[0].P) c *= l;
		for (int i=0; i<b.size(); ++i) for (int j=Rs.size(); j<bd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<w.size(); ++i) for (int j=Rs.size(); j<wd[i]; ++j) Rs.push_back(Rs.back().derivative());
		for (int i=0; i<f.size(); ++i) for (int j=Rs.size(); j<fd[i]; ++j) Rs.push_back(Rs.back().derivative());
	}

	void normalize () {
		RationalFraction R;
		for (int i=0; i<b.size(); ++i) for (int j=0; j<bd[i]; ++j) R.add_root(b[i]);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<fd[i]; ++j) R.add_pole(f[i]);
		cpx avg = 0; int d=0;
		for (int i=0; i<w.size(); ++i) { d += wd[i]; avg += R(w[i])*cpx(wd[i]); }
		l = cpx(d)/avg;
		compute();
	}

	double belyi () {
		cpx z0 = b[0];                 	for (auto & z : b) z -= z0; for (auto & z : w) z -= z0; for (auto & z : f) z -= z0;
		cpx r0 = polar(1.0,-arg(w[0]));	for (auto & z : b) z *= r0; for (auto & z : w) z *= r0; for (auto & z : f) z *= r0;
		normalize();

		double lambda = pow(abs(Rs[0].P.back()),1.0/Rs[0].degree());
		for (auto & z : b) z *= lambda; for (auto & z : w) z *= lambda; for (auto & z : f) z *= lambda; normalize();

		double err = sqrt(cost());
		cout << setprecision (log10(1/err)-1) << fixed; Rs[0].P.precision = err; Rs[0].Q.precision = err;
		return lambda;
	}

	double cost() {
		double out=0;
		for (int i=0; i<w.size(); ++i) for (int j=0; j<wd[i]; ++j) out += norm (Rs[j](w[i]) - (j==0?1.0:0.0));
		return out;
	}

	double cost (const vector<double> & xy) {
		int n1 = b.size(), n2 = w.size(), n3 = f.size();

		for (int i=0; i<n1; ++i) b[i] = cpx (xy[2*i],xy[2*i+1]);
		for (int i=0; i<n2; ++i) w[i] = cpx (xy[2*n1+2*i],xy[2*n1+2*i+1]);
		for (int i=0; i<n3; ++i) f[i] = cpx (xy[2*n1+2*n2+2*i],xy[2*n1+2*n2+2*i+1]);
		l = cpx (xy[2*n1+2*n2+2*n3],xy[2*n1+2*n2+2*n3+1]);

		compute(); return cost();
	}

	void find () {
		vector<double> bw;
		for (auto z : b) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : w) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		for (auto z : f) { bw.push_back(real(z)); bw.push_back(imag(z)); }
		bw.push_back(real(l)); bw.push_back(imag(l));

		double c = cost(bw), eps = sqrt(c), nc = c;
		while (eps>1e-100) {
			cerr << c << " (" << eps << ")          \r";
			bool flag = false;
			for (int i=0; i<bw.size(); ++i) {
				bw[i] += eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { bw[i] -= eps; }
				bw[i] -= eps; nc = cost(bw); if (nc<c) { c=nc; flag=true; } else { bw[i] += eps; }
			}
			if (!flag) eps /= 2;
		}
		cerr << endl;
	}

	vector<cpx>     	b,w,f;
	vector<unsigned>	bd,wd,fd;
	cpx             	l=1.0;

	vector<RationalFraction>	Rs;
};

ostream & operator<< (ostream & os, const Constellation & C) {
	os << "Black vertices / zeros: " << endl;
	for (int i=0; i<C.b.size(); ++i) { os << "| " << C.bd[i] << "\t" << C.b[i]; for (int j=0; j<C.bd[i]; ++j) os << "\t" << C.Rs[j](C.b[i]); os << endl; }
	os << endl;
	os << "White vertices / ones: " << endl;
	for (int i=0; i<C.w.size(); ++i) { os << "| " << C.wd[i] << "\t" << C.w[i]; for (int j=0; j<C.wd[i]; ++j) os << "\t" << C.Rs[j](C.w[i]); os << endl; }
	os << endl;
	os << "Red vertices / poles: " << endl;
	for (int i=0; i<C.f.size(); ++i) { os << "| " << C.fd[i] << "\t" << C.f[i]; for (int j=0; j<C.fd[i]; ++j) os << "\t" << C.Rs[j](C.f[i]); os << endl; }
	os << endl;
	os << C.Rs[0] << endl;
	return os;
}

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} };                                	// Graph tripod
	// Hypermap M { {{0,1,2}}, {{0},{1},{2}}, {{2,1,0}} };                                                        	// Dessin tripod
	// Hypermap M { {{0,1,2,3}}, {{0},{1},{2},{3}}, {{3,2,1,0}} };                                                	// Dessin 4-star
	// Hypermap M { {{0,1,2,3,4}}, {{0},{1},{2},{3},{4}}, {{4,3,2,1,0}} };                                        	// Dessin 5-star
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} };                	// Long graph tripod - take n=4
	// Hypermap M { {{2,1,0},{3}}, {{2},{1},{0,3}}, {{2,0,3,1}} };                                                	// Long dessin tripod - Lando page 87
	// Hypermap M { {{0,1,2},{3,4},{5,6}}, {{0},{1,3},{2,5},{4},{6}}, {{0,2,6,5,1,4,3}} };                        	// Long-legged tripod - Lando page 88
	// Hypermap M { {{4,1,2},{3,0},{5,6}}, {{4,3},{1},{2},{0,5},{6}}, {{4,0,6,5,3,2,1}} };                        	// Long-tailed tripod - Lando page 89 - FAIL?
	// Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} };	// Graph H - FAIL?
	// Hypermap M { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} };                                        	// Dessin H
	Hypermap M { {{0,2,3},{1}}, {{0,1},{2,3}}, {{0,1,3},{2}} };                                                   	// Simple map - Lando page 107

	cout << M;
	cout << "  sigma: " << M.sigma << endl;
	cout << "  alpha: " << M.alpha << endl;
	cout << "    phi: " << M.phi << endl;
	cout << endl;

	if (!H['d']) M.dessin();

	for (int i=0; i<int(H['n']); ++i) M.split_edges(); Spheroidal T (M,H); T.pack(); cerr << endl;

	unsigned i=0; while (!(T.initial[i]&8)) ++i; auto & v = T.V[T.E[i].src]; T.linear (1,-v.z); T.inversion(); T.linear (-1/v.r,0); T.output_pdf();
	{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); } T.linear (std::polar(1.0,-T.E[0].a)); T.output_pdf();

	Constellation C (T);

	double l = C.belyi(); T.linear(l); T.output_pdf();
	C.find(); C.belyi(); cout << endl << C;
}
