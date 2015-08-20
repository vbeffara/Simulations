#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/Hub.h>

using namespace vb;

class Site { public:
	Site (int d_ = 0) : d(d_) {}

	operator Color () { static std::vector<Color> key = { BLACK, RED, GREEN }; return key[d]; }

	int d;
};

class Nematic : public vb::Bitmap<Site> { public:
	Nematic (int n_, int k_, double b_) : Bitmap<Site> (n_,n_), k(k_), b(b_), P(n_,0) {};

	void prec () {
		ok = std::min(k,w()); ob = b; z = exp(2*ob); std::vector<double> Z(w(),1);
		for (int i=ok; i<Z.size(); ++i) Z[i] = Z[i-1] + z * Z[i-ok];
		for (int i=ok; i<P.size(); ++i) P[i] = z * Z[i-ok] / Z[i];
		dd = ok * z * Z[w()-ok] / (Z[w()-1] + ok * z * Z[w()-ok]);
	}

	void add (coo z, int d) { (d==1 ? nh : nv) += ok; for (int i=0; i<ok; ++i) { atp(z) = d; z += dz[d-1]; } }

	void fill (coo z, int d, int l) {
		while (l >= ok) {
			if (prng.bernoulli(P[l])) { add (z,d); z += dz[d-1] * ok; l -= ok; }
			else { z += dz[d-1]; l--; }
		}
	}

	void re_line (int y) {
		bool all_h = true;
		for (int x=0; x<w(); ++x) {
			if (at(coo(x,y)).d == 2) all_h = false;
			if (at(coo(x,y)).d == 1) { atp(coo(x,y)) = 0; nh --; }
		}
		if (all_h) {
			if (prng.bernoulli(dd)) {
				int x = prng.uniform_int(ok); add (coo(-x,y), 1); fill (coo(ok-x,y), 1, w()-ok);
			} else fill (coo(1,y), 1, w()-1);
		} else {
			int i0=0; while (atp(coo(i0,y)).d != 2) ++i0;
			int i=i0; while (i < i0+w()) {
				++i;
				if (atp(coo(i,y)).d == 0) { int j=i; while (atp(coo(j,y)).d == 0) ++j; fill (coo(i,y), 1, j-i); i=j; }
			}
		}
		step();
	}

	void re_col (int x) {
		bool all_v = true;
		for (int y=0; y<h(); ++y) {
			if (at(coo(x,y)).d == 1) all_v = false;
			if (at(coo(x,y)).d == 2) { at(coo(x,y)) = 0; nv --; }
		}
		if (all_v) {
			if (prng.bernoulli(dd)) {
				int y = prng.uniform_int(ok); add (coo(x,-y), 2); fill (coo(x,ok-y), 2, h()-ok);
			} else fill (coo(x,1), 2, h()-1);
		} else {
			int i0=0; while (atp(coo(x,i0)).d != 1) ++i0;
			int i=i0; while (i < i0+h()) {
				++i;
				if (atp(coo(x,i)).d == 0) { int j=i; while (atp(coo(x,j)).d == 0) ++j; fill (coo(x,i), 2, j-i); i=j; }
			}
		}
		step();
	}

	void go () {
		Console C;          	C.watch (order, "order");
		C.watch (b, "beta");	C.manage (b,-5.0,5.0);
		C.watch (k, "k");   	C.manage (k,1,100);
		show(); C.show(); if (H['v']) snapshot_setup("movie",10);
		for (int t=int(H['t'])-1 ; t!=0 ; --t) {
			if ((k != ok) || (b != ob)) prec();
			for (int i=0; i<w(); ++i) re_col(i);
			for (int i=0; i<h(); ++i) re_line(i);
			order = nh+nv>0 ? double (nh-nv) / double (nh+nv) : 0;
			if ((!(t%100)) && H['l']) std::cout << order << std::endl;
		}
	};

	int k, ok=0, nh=0, nv=0;
	double b, ob=0, z, order=0, dd;
	std::vector<double> P;
};

int main (int argc, char ** argv) {
	H.init ("Nematic system on the square lattice", argc,argv, "n=500,k=20,b=-2,v,l,t=0");
	Nematic(H['n'],H['k'],H['b']).go();
}
