#include <vb/PRNG.h>
#include <vb/TriMatrix.h>
#include <vb/coo.h>

using namespace vb; using namespace std;

class Cluster : public TriMatrix<bool> { public:
	Cluster (coo ul_ = {0,0}, int w_ = bs) : TriMatrix<bool> (false), ul(ul_), w(w_), np(0) {};

	bool fits (coo z) const { return (z.x >= ul.x) && (z.y >= ul.y) && (z.x < ul.x + w) && (z.y < ul.y + w); }

	void ensure_sub () {
		if (sub.size()>0) return;
		int ww = w/3;
		for (int y=0; y<3; ++y) for (int x=0; x<3; ++x) sub.push_back (new Cluster (ul + coo(x*ww,y*ww), ww));
	}

	void grow () {
		auto bak = sub; sub.clear();
		ul.x -= w; ul.y -= w; w *= 3;
		ensure_sub();
		Cluster &c = * sub[4];
		tile.swap(c.tile); c.np = np; c.sub = bak;
	};

	bool at (coo z) {
		return TriMatrix<bool>::at (z);
	};

	void insert (coo z) {
		if (at(z)) return;
		while (!fits(z)) grow();
		if (np == w*w) return;
		if (w == bs) {
			if (np == 0) tile.resize (w*w,false);
			int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y;
			if (!tile[xy]) { np++; tile [xy] = true; }
		} else {
			int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
			ensure_sub(); ++np; sub[i+3*j] -> insert(z);
		}
		put (z,true);
	};

	void remove (coo z) {
		if (!at(z)) return;
		while (!fits(z)) grow();
		if (np == 0) return;
		if (w == bs) {
			if (np == w*w) tile.resize (w*w,true);
			int x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y;
			if (tile[xy]) { np--; tile[xy] = false; }
		} else {
			int ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww;
			ensure_sub(); --np; sub[i+3*j] -> remove(z);
		}
		put (z,false);
	};

	void dump (string pre = "") {
		if (np==0) { if (pre=="") cerr << "EMPTY" << endl; return; }

		cerr << pre << np << " in " << ul << "[" << w << "]" << endl;
		if ((np>0) && (np < w*w)) {
			if (w == bs) {
				for (int y=0; y<w; ++y) for (int x=0; x<w; ++x) if ((np==w*w) || (tile[x+w*y])) cerr << pre << "    " << ul + coo(x,y) << endl;
			} else {
				for (auto c : sub) c->dump (pre + "    ");
			}
		}
	}

	coo ul;
	long w,np;
	vector<bool> tile;
	vector<Cluster *> sub;
	static int bs;
};

int Cluster::bs = 10;

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

	Cluster D;
	for (int t=0; D.np<2500; ++t) {
		if (!(t%100)) D.dump();
		D.insert ({-10+prng.uniform_int(50),-10+prng.uniform_int(50)});
	}
	for (int t=0; D.np>0; ++t) {
		if (!(t%100)) D.dump();
		D.remove ({-10+prng.uniform_int(50),-10+prng.uniform_int(50)});
	}
}
