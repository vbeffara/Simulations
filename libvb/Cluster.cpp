#include <vb/Cluster.h>

namespace vb {
	bool Cluster::fits (coo z) const {
		return (z.x >= ul.x) && (z.y >= ul.y) && (z.x < ul.x + w) && (z.y < ul.y + w);
	}

	void Cluster::ensure_sub () {
		if (!sub.empty()) return;
		long ww = w/3;
		for (long y=0; y<3; ++y) for (long x=0; x<3; ++x) sub.emplace_back (ul + coo(x*ww,y*ww), ww);
		if (np == w*w) for (auto & c : sub) c.np = ww*ww;
	}

	void Cluster::grow () {
		if (np != 0) {
			std::vector <Cluster> bak; bak.swap(sub);
			ul.x -= w; ul.y -= w; w *= 3; ensure_sub();
			Cluster &c = sub[4]; tile.swap(c.tile); c.np = np; c.sub.swap(bak);
		} else {
			ul.x -= w; ul.y -= w; w *= 3;
		}
	}

	bool Cluster::at (coo z) const {
		if ((np == 0) || (!fits(z))) return false;
		if (np == w*w) return true;
		if (!sub.empty()) return sub[sub_index(z)].at(z);
		long x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y; return tile[xy];
	}

	void Cluster::put (coo z, bool b) {
		long plain = w*w*static_cast<long>(b); if (np == plain) return;
		if (w == bs) {
			if (tile.empty()) tile.resize (w*w,!b);
			long x = z.x - ul.x, y = z.y - ul.y, xy = x + w*y;
			np += (b?1:-1); tile [xy] = b;
			if (np == plain) std::vector<bool>().swap(tile);
		} else {
			ensure_sub(); np += (b?1:-1); sub[sub_index(z)].put(z,b);
			if (np == plain) std::vector<Cluster>().swap(sub);
		}
	}

	void Cluster::insert (coo z) { while (!fits(z)) grow(); if (!at(z)) put (z,true); }

	void Cluster::remove (coo z) { if (at(z)) put (z,false); }

	void Cluster::validate () {
		if ((np==0) || (np==w*w)) { assert (!tile.size()); assert (!sub.size()); }
		if (!sub.empty()) {
			long s=0; for (auto & c : sub) { c.validate(); s += c.np; } assert (s==np);
		}
	}

	void Cluster::paint (Image & I, coo ul, coo br) {
		if (ul==br) { ul = {0,0}; br = {I.w(),I.h()}; }
		if (br.x-ul.x > w) { ul = (ul + br - coo(w,w)) / 2; br = ul + coo(w,w); }
		if (np == 0) { for (long x=ul.x; x<br.x; ++x) for (long y=ul.y; y<br.y; ++y) I.put(coo(x,y),BLACK); }
		else if (np == w*w) {
			for (long x=ul.x; x<br.x; ++x) for (long y=ul.y; y<br.y; ++y)
				I.put (coo(x,y), (x==ul.x)||(x==br.x-1)||(y==ul.y)||(y==br.y-1) ? RED : BLUE); }
		else if (br == ul + coo(1,1)) { I.put (ul, Grey((255*np) / (w*w))); }
		else if (!sub.empty()) {
			long ww = br.x-ul.x, hh = br.y-ul.y;
			for (long x=0; x<3; ++x) for (long y=0; y<3; ++y)
				sub[x+3*y].paint (I,ul+coo(x*ww/3,y*hh/3),ul+coo((x+1)*ww/3,(y+1)*hh/3));
		} else {
			long ww = br.x-ul.x, s = w/ww;
			for (long x=0; x<ww; ++x) for (long y=0; y<ww; ++y)
				I.put (ul+coo(x,y), tile[x*s+w*y*s] ? WHITE : BLACK);
		}
	}

	long Cluster::sub_index (coo z) const { long ww = w/3, i = (z.x-ul.x)/ww, j = (z.y-ul.y)/ww; return i+3*j; }

	long Cluster::bs = 16;
} // namespace vb
