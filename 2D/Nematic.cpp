#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb;

template<> Color vb::to_Color (int t) {
	static const std::vector<Color> key = { BLACK, RED, GREEN };
	return key[t];
}

class Nematic : public vb::Bitmap<int> { public:
	Nematic (int n_, int m_, int k_, double b_) : Bitmap<int> (n_,m_), k(k_), b(b_), P(std::max(n_,m_),0) {};

	void prec () {
		ok = std::min(k,std::min(w(),h())); ob = b; std::vector<double> Z(P.size());
		double zz = exp(2*ob/double(ok)); for (int i=0; i<ok; ++i) Z[i] = pow(zz,-i);
		for (int i=ok; i<Z.size(); ++i) Z[i] = Z[i-1] / zz + Z[i-ok];
		for (int i=ok; i<P.size(); ++i) P[i] = Z[i-ok] / Z[i];
		dd = ok * Z[Z.size()-ok] / (Z[Z.size()-1] / zz + ok * Z[Z.size()-ok]);
	}

	void add (coo z, int d) { for (int i=0; i<ok; ++i) { atp(z) = d; z += dz[d-1]; } }

	void fill (coo z, int d, int l) {
		while (l >= ok) {
			if (prng.bernoulli(P[l])) { add (z,d); z += dz[d-1] * ok; l -= ok; }
			else { z += dz[d-1]; l--; }
		}
	}

	void redo (coo z, int d) {
		bool empty = true;
		int hw = (d==1 ? w() : h());
		for (int x=0; x < hw; z+=dz[d-1], ++x) {
			if (at(z) == 3-d) empty = false;
			if (at(z) == d) at(z) = 0;
		}
		if (empty) {
			if (prng.bernoulli(dd)) { z -= dz[d-1] * prng.uniform_int(ok); add (z,d); fill (z + dz[d-1] * ok, d, hw-ok); }
			else fill (z + dz[d-1], d, hw-1);
		} else {
			while (atp(z) != 3-d) { z += dz[d-1]; } z += dz[d-1];
			for (coo zz = z - dz[d-1]*hw; zz != z; zz += dz[d-1]) {
				int l=0; while (atp(zz) == 0) { ++l; zz += dz[d-1]; } if (l>0) fill (zz - dz[d-1]*l, d, l);
			}
		}
		step();
	}

	void go () {
		Console C;
		C.watch (order, "order");
		C.manage (b,-5.0,10.0,"beta");
		C.manage (k,1,100,"k");
		show(); C.show(); if (H['v']) snapshot_setup("movie",10);
		for (int t=int(H['t'])-1 ; t!=0 ; --t) {
			if ((k != ok) || (b != ob)) prec();
			for (int i=0; i<h(); ++i) redo (coo(0,i), 1);
			for (int i=0; i<w(); ++i) redo (coo(i,0), 2);
			int nh = 0; for (auto v : *this) if (v == 1) ++nh;
			int nv = 0; for (auto v : *this) if (v == 2) ++nv;
			order = nh+nv>0 ? double (nh-nv) / double (nh+nv) : 0;
			if (((t%100) == 0) && H['l']) std::cout << order << std::endl;
		}
	};

	int k, ok=0;
	double b, ob=0, order=0, dd=0;
	std::vector<double> P;
};

int main (int argc, char ** argv) {
	H.init ("Nematic system on the square lattice", argc,argv, "n=500,m=0,k=7,b=2,v,l,t=0");
	Nematic (H['n'], int(H['m']) != 0 ? H['m'] : H['n'], H['k'], H['b']).go();
}
