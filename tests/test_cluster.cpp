#include <vb/PRNG.h>
#include <vb/coo.h>

using namespace vb; using namespace std;

class Cluster { public:
	Cluster (coo ul_ = {0,0}, int w_ = bs) : ul(ul_), w(w_), np(0) {};

	bool fits (coo z) const { return (z.x >= ul.x) && (z.y >= ul.y) && (z.x < ul.x + w) && (z.y < ul.y + w); }

	void ensure_sub () {
		if (sub.size()>0) return;
		int ww = w/3;
		for (int y=0; y<3; ++y) for (int x=0; x<3; ++x) sub.push_back (new Cluster (ul + coo(x*ww,y*ww), ww));
		if (np == w*w) for (auto c : sub) c->np = ww*ww;
	}

	void grow () {
		if (np > 0) {
			vector <Cluster *> bak; bak.swap(sub);
			ul.x -= w; ul.y -= w; w *= 3; ensure_sub();
			Cluster &c = * sub[4]; tile.swap(c.tile); c.np = np; c.sub.swap(bak);
		} else {
			ul.x -= w; ul.y -= w; w *= 3;
		}
	};

	bool at (coo z) const {
		if ((np == 0) || (!fits(z))) return false;
		if (np == w*w) return true;
		if (w == bs) { int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y; return tile[xy]; }
		else { int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww; return sub[i+3*j] -> at(z); }
	};

	void put (coo z, bool b, bool quick = false) {
		if (!quick) { if (at(z) == b) return; while (!fits(z)) grow(); }
		if (w == bs) {
			if (!tile.size()) tile.resize (w*w,!b);
			int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y;
			np += (b?1:-1); tile [xy] = b;
			if (np == (b ? w*w : 0)) vector<bool>().swap(tile);
		} else {
			int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
			ensure_sub(); np += (b?1:-1); sub[i+3*j] -> put(z,b,true);
			if (np == (b ? w*w : 0)) vector<Cluster*>().swap(sub);
		}
	}

	void insert (coo z) { put (z,true); }

	void remove (coo z) { put (z,false); }

	void validate () {
		if ((np==0) || (np==w*w)) { assert (!tile.size()); assert (!sub.size()); }
		if (sub.size()) {
			int s=0; for (auto c : sub) { c -> validate(); s += c -> np; } assert (s==np);
		}
	}

	void dump (string pre = "") {
		validate ();
		if (np == 0) { if (pre=="") cerr << "EMPTY" << endl; return; }
		if (np == w*w) { cerr << pre << "FULL " << ul << "[" << w << "]" << endl; return; }

		cerr << pre << np << " in " << ul << "[" << w << "]" << endl;
		if ((np > 0) && (np < w*w)) {
			for (auto c : sub) c->dump (pre + "    ");
		}
	}

	coo ul;
	long w,np;
	vector <bool> tile;
	vector <Cluster *> sub;
	static int bs;
};

int Cluster::bs = 32;

int main () {
	Cluster C;
	C.dump();
	C.insert ({35,42});
	C.dump();
	C.insert ({1234,5678});
	C.dump();
	C.insert ({91823749,-2793474});
	C.dump();
	C.remove ({1234,5678});
	C.dump();

	assert (C.at({35,42}));
	assert (!C.at({1234,5678}));
	assert (C.at({91823749,-2793474}));
	assert (!C.at({3,4}));
	assert (!C.at({981327,2371827}));

	Cluster D; int l = 1000;
	for (int t=0; D.np<l*l; ++t) {
       D.insert ({prng.uniform_int(l),prng.uniform_int(l)});
	}
	D.dump();
	for (int t=0; D.np>0; ++t) {
       D.remove ({prng.uniform_int(l),prng.uniform_int(l)});
	}
	D.dump();
}
