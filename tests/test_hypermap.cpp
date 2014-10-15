#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Polynomial.h>
#include <vb/Spheroidal.h>
#include <cassert>
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

double cost (vector<double> xy, const Spheroidal & T) {
	vector<pair<cpx,unsigned>> black = roots(T);
	vector<pair<cpx,unsigned>> white = ones(T);
	int n1 = black.size(), n2 = white.size(); assert (xy.size()==2*(n1+n2)+2);
	for (int i=0; i<n1; ++i) { black[i].first = cpx (xy[2*i],xy[2*i+1]); }
	for (int i=0; i<n2; ++i) { white[i].first = cpx (xy[2*n1+2*i],xy[2*n1+2*i+1]); }

	Polynomial P ({1}); for (auto z : black) for (int i=0; i<z.second; ++i) P.add_root(z.first);

	for (auto & c : P) c *= cpx (xy[2*n1+2*n2],xy[2*n1+2*n2+1]);

	vector<Polynomial> Ps (1,P); for (int i=0; i<P.degree(); ++i) Ps.push_back(Ps.back().derivative());
	double out=0;
	for (auto z : white) for (int i=0; i<z.second; ++i) out += norm (Ps[i](z.first) - (i==0?1.0:0.0));
	return out;
}

pair<vector<cpx>,vector<cpx>> find (vector<cpx> bs, vector<cpx> ws, const cpx l, const Spheroidal & T) {
	vector<double> bw;
	for (auto b : bs) { bw.push_back(real(b)); bw.push_back(imag(b)); }
	for (auto w : ws) { bw.push_back(real(w)); bw.push_back(imag(w)); }
	bw.push_back(real(l)); bw.push_back(imag(l));
	double c = cost (bw,T), old_c = c+1, eps = sqrt(c), nc = c;

	while (eps>1e-100) {
		cout << c << " (" << eps << ")          \r";
		old_c = c; bool flag = false;
		for (int i=0; i<bw.size(); ++i) {
			bw[i] += eps; nc = cost (bw,T); if (nc<c) { c=nc; flag=true; } else { bw[i] -= eps; }
			bw[i] -= eps; nc = cost (bw,T); if (nc<c) { c=nc; flag=true; } else { bw[i] += eps; }
		}
		if (!flag) eps /= 2;
	}

	int nb = bs.size();
	for (int i=0; i<bs.size(); ++i) bs[i] = cpx (bw[2*i],bw[2*i+1]);
	for (int i=0; i<ws.size(); ++i) ws[i] = cpx (bw[2*nb+2*i],bw[2*nb+2*i+1]);
	return {bs,ws};
}

void belyi (Spheroidal & T) {
	Polynomial P ({1}); for (auto z : roots(T)) for (int i=0; i<z.second; ++i) P.add_root(z.first);

	cpx avg = 0, avg2 = 0; int d=0;
	for (auto z : ones(T)) { d += z.second; avg += P(z.first)*cpx(z.second); avg2 += P(z.first)*P(z.first)*cpx(z.second); }
	avg /= d; avg2 /= d; for (auto & c : P) c /= avg;

	cpx lead = P.back(); double lambda = pow(abs(lead),1.0/(P.size()-1));
	for (int i=1; i<P.size(); ++i) P[i] /= pow(lambda,i);
	T.linear(lambda); T.output_pdf();

	vector<Polynomial> Ps (1,P); for (int i=0; i<P.degree(); ++i) Ps.push_back(Ps.back().derivative());

	double err=0;
	for (auto z : roots(T)) for (int i=0; i<z.second; ++i) err += abs(Ps[i](z.first));
	for (auto z : ones(T))  for (int i=0; i<z.second; ++i) err += abs(Ps[i](z.first) - (i==0?1.0:0.0));

	for (auto & c : P) {
		if (fabs(real(c))<err) c = cpx(0,imag(c));
		if (fabs(imag(c))<err) c = cpx(real(c),0);
	}

	cout << "Remaining error: " << err << endl;
	cout << setprecision (log10(1/err));
	cout << endl << "Black vertices:" << endl;

	for (auto z : roots(T)) {
		cout << "  " << z.second << "\t" << fixed << z.first << scientific;
		for (int i=0; i<z.second; ++i) { cout << "\t" << Ps[i](z.first); err += abs(Ps[i](z.first)); }
		cout << endl;
	}
	cout << endl << "White vertices:" << endl;
	for (auto z : ones(T)) {
		cout << "  " << z.second << "\t" << fixed << z.first << scientific;
		for (int i=0; i<z.second; ++i) { cout << "\t" << Ps[i](z.first) - (i==0?1.0:0.0); err += abs(Ps[i](z.first) - (i==0?1.0:0.0)); }
		cout << endl;
	}

	for (auto & c : P) {
		if (fabs(real(c))<err) c = cpx(0,imag(c));
		if (fabs(imag(c))<err) c = cpx(real(c),0);
	}

	// cout << setprecision (log10(1/err)+2);
	cout << endl << fixed << P << endl;
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

	unsigned i=0; while (!(T.initial[i]&8)) ++i; auto & v = T.V[T.E[i].src];
	T.linear (1,-v.z); T.inversion(); T.linear (-1/v.r,0); T.output_pdf();

	{ cpx z; while ((z = T.V[T.E[0].src].z) != 0.0) T.mobiusto0 (z); }
	T.linear (std::polar(1.0,-T.E[0].a)); T.output_pdf();

	Polynomial P ({1});
	for (auto z : roots(T)) for (int i=0; i<z.second; ++i) P.add_root(z.first);

	cpx avg = 0, avg2 = 0; int d=0;
	for (auto z : ones(T)) { d += z.second; avg += P(z.first)*cpx(z.second); avg2 += P(z.first)*P(z.first)*cpx(z.second); }
	avg /= d; avg2 /= d; for (auto & c : P) c /= avg;

	cpx lead = P.back(); double lambda = pow(abs(lead),1.0/(P.size()-1));
	for (int i=1; i<P.size(); ++i) P[i] /= pow(lambda,i);
	T.linear(lambda); T.output_pdf();

	vector<Polynomial> Ps (1,P); for (int i=0; i<P.degree(); ++i) Ps.push_back(Ps.back().derivative());
	vector<cpx> bs,ws;

	cout << endl << P << endl;
	cout << endl << "Black vertices:" << endl;
	for (auto z : roots(T)) {
		bs.push_back(z.first);
		cout << "  " << z.second << "\t" << z.first;
		for (int i=0; i<z.second; ++i) cout << "\t" << Ps[i](z.first);
		cout << endl;
	}
	cout << endl << "White vertices:" << endl;
	for (auto z : ones(T)) {
		ws.push_back(z.first);
		cout << "  " << z.second << "\t" << z.first;
		for (int i=0; i<z.second; ++i) cout << "\t" << Ps[i](z.first) - (i==0?1.0:0.0);
		cout << endl;
	}

	cout << endl;
	auto coo = find(bs,ws,P[P.degree()],T);
	auto oldroots = roots(T); for (auto & v : T.V) for (int i=0; i<oldroots.size(); ++i) if (oldroots[i].first == v.z) v.z = coo.first[i];
	auto oldones  = ones(T);  for (auto & v : T.V) for (int i=0; i<oldones.size();  ++i) if (oldones[i].first  == v.z) v.z = coo.second[i];

	T.linear (1.0,-roots(T)[0].first);
	if (roots(T).size()>1) T.linear (polar(1.0,-arg(roots(T)[1].first)));
	else T.linear (polar(1.0,-arg(ones(T)[0].first)));

	belyi(T);
}
