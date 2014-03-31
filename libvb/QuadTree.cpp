#include <vb/QuadTree.h>

namespace vb {
	QuadTree::QuadTree (coo UL, coo BR, int M) :
		n(0), ul(UL), br(BR), center((ul+br)/2), iul(br), ibr(ul), m(M) {};

	QuadTree::~QuadTree () {
		if (n>m) for (int i=0; i<4; ++i) delete children[i];
	}

	int QuadTree::index (coo z) const {
		if (z.y<center.y)	{ if (z.x<center.x) return 0; else return 1; }
		else             	{ if (z.x<center.x) return 2; else return 3; }
	}

	void QuadTree::insert (coo z) {
		iul.x = std::min(iul.x,z.x); iul.y = std::min(iul.y,z.y);
		ibr.x = std::max(ibr.x,z.x); ibr.y = std::max(ibr.y,z.y);
		if (n==m) split();
		if (n<m) pts.push_back(z); else children[index(z)] -> insert (z);
		++n;
	}

	void QuadTree::split () {
		children[0] = new QuadTree (ul,                	center,            	m);
		children[1] = new QuadTree (coo(center.x,ul.y),	coo(br.x,center.y),	m);
		children[2] = new QuadTree (coo(ul.x,center.y),	coo(center.x,br.y),	m);
		children[3] = new QuadTree (center,            	br,                	m);
		for (auto & z : pts) children[index(z)] -> insert (z);
		std::vector<coo>().swap(pts);
	}

	int QuadTree::idist (coo z) const { return std::min ({z.x-ul.x, 	z.y-ul.y, 	br.x-z.x, 	br.y-z.y}); 	}
	int QuadTree::odist (coo z) const { return std::max ({z.x-ibr.x,	z.y-ibr.y,	iul.x-z.x,	iul.y-z.y});	}

	void QuadTree::nn (coo z, QuadIndex & qi) const {
		if (n<=m) for (auto & w : pts) {
			int newnorm = sup (z-w);
			if (newnorm < qi.d) { qi.d = newnorm; qi.z = w; }
		} else {
			int i0 = index(z); if (children[i0] -> n) children[i0] -> nn (z,qi);
			for (int i=0; i<4; ++i) if (i != i0) {
				QuadTree *q = children[i]; if ((q -> n) && (q -> odist(z) < qi.d))
					children[i] -> nn (z,qi);
			}
		}
	}
}
