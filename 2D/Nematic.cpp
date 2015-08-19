#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/Hub.h>

using namespace vb;

class Site { public:
	Site (int d_ = 0, int x_ = 0, int k_ = 0) : d(d_), x(x_), k(k_) {}

	operator Color () { static std::vector<Color> key = { BLACK, RED, GREEN }; return key[d]; }

	int d, x, k;
};

class Nematic : public vb::Bitmap<Site> { public:
	Nematic (int n_, int k_, double b_) : Bitmap<Site> (n_,n_), k(k_), b(b_), Z(n_) {};

	void prec() {
		ok = std::min(k,w()); ob = b; p = exp(ob) / (exp(ob) + exp(-ob)); z = exp(2*ob);
		for (int i=0; i<ok; ++i) if (i<Z.size()) Z[i] = 1;
		for (int i=ok; i<Z.size(); ++i) Z[i] = Z[i-1] + z * Z[i-ok];
		ZZ = Z[w()-1] + ok * z * Z[w()-ok];
		dd = 1.0 - Z[w()-1] / ZZ;
	}

	bool fits	(coo z, int d, int k) { for (int i=0; i<k; ++i) { if (atp(z).d > 0) return false; z += dz[d-1]; } return true; }
	void add 	(coo z, int d, int k) { (d==1 ? nh : nv) += k; for (int i=0; i<k; ++i) { atp(z) = {d,i,k}; z += dz[d-1]; } }
	void del 	(coo z, int d, int k) { (d==1 ? nh : nv) -= k; for (int i=0; i<k; ++i) { atp(z) = {0,0,0}; z += dz[d-1]; } }

	void fill_h (int i, int j, int y) {
		if (j-i<ok) return;
		if (prng.bernoulli(z*Z[j-i-ok] / Z[j-i])) { add (coo(i,y),1,ok); fill_h(i+ok,j,y); } else { fill_h(i+1,j,y); }
	}

	void fill_v (int i, int j, int x) {
		if (j-i<ok) return;
		if (prng.bernoulli(z*Z[j-i-ok] / Z[j-i])) { add (coo(x,i),2,ok); fill_v(i+ok,j,x); } else { fill_v(i+1,j,x); }
	}

	void re_line (int y) {
		bool all_h = true;
		for (int x=0; x<w(); ++x) {
			if (at(coo(x,y)).d == 2) all_h = false;
			if ((at(coo(x,y)).d == 1) && (at(coo(x,y)).x == 0)) del (coo(x,y),1,at(coo(x,y)).k);
		}
		if (all_h) {
			if (prng.bernoulli(dd)) {
				int x = prng.uniform_int(ok); add (coo(-x,y), 1, ok); fill_h (ok-x, w()-x, y);
			} else fill_h (1,w(),y);
		} else {
			int i0=0; while (atp(coo(i0,y)).d != 2) ++i0;
			int i=i0; while (i < i0+w()) {
				++i;
				if (atp(coo(i,y)).d == 0) { int j=i; while (atp(coo(j,y)).d == 0) ++j; fill_h(i,j,y); i=j; }
			}
		}
		step();
	}

	void re_col (int x) {
		bool all_v = true;
		for (int y=0; y<h(); ++y) {
			if (at(coo(x,y)).d == 1) all_v = false;
			if ((at(coo(x,y)).d == 2) && (at(coo(x,y)).x == 0)) del (coo(x,y),2,at(coo(x,y)).k);
		}
		if (all_v) {
			if (prng.bernoulli(dd)) {
				int y = prng.uniform_int(ok); add (coo(x,-y), 2, ok); fill_v (ok-y, h()-y, x);
			} else fill_v (1,h(),x);
		} else {
			int i0=0; while (atp(coo(x,i0)).d != 1) ++i0;
			int i=i0; while (i < i0+h()) {
				++i;
				if (atp(coo(x,i)).d == 0) { int j=i; while (atp(coo(x,j)).d == 0) ++j; fill_v(i,j,x); i=j; }
			}
		}
		step();
	}

	void go () {
		Console C;
		C.watch (nh, "horizontal");  	C.watch (nv, "vertical");
		C.watch (density, "density");	C.watch (order, "order");
		C.watch (b, "beta");         	C.manage (b,-5.0,5.0);
		C.watch (k, "k");            	C.manage (k,1,100);
		show(); C.show(); if (H['v']) snapshot_setup("movie",10);
		for (int t=0; t<5e12; ++t) {
			if ((k != ok) || (b != ob)) prec();
			if (H['g']) {
				coo z = rand();
				if (prng.bernoulli(p)) { int d = prng.bernoulli(.5) + 1; if (fits (z,d,ok)) add (z,d,ok); }
				else { if ((at(z).d > 0) && (at(z).x == 0)) del (z,at(z).d,at(z).k); }
				step();
			} else {
				for (int i=0; i<w(); ++i) { re_line(i); re_col(i); }
			}
			density = double (nh+nv) / (w()*h()) / dd;
			order = nh+nv>0 ? double (nh-nv) / double (nh+nv) : 0;
		}
	};

	int k, ok=0, nh=0, nv=0;
	double b, ob=-10, p, z, density=0, order=0, ZZ, dd;
	std::vector<double> Z;
};

int main (int argc, char ** argv) {
	H.init ("Nematic system on the square lattice", argc,argv, "n=500,k=20,b=-2,g,v");
	Nematic(H['n'],H['k'],H['b']).go();
}
