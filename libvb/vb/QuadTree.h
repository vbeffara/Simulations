#pragma once

#include <vb/coo.h>
#include <vector>
#include <iostream>

namespace vb {
	struct QuadIndex { coo z; int d; };

	class QuadTree { public:
		QuadTree (coo UL, coo BR, int M, QuadTree * p = 0) : ul(UL), br(BR), center((ul+br)/2), parent(p), n(0), m(M), leaf(true) {};
		~QuadTree () { if (!leaf) for (int i=0; i<4; ++i) delete children[i]; }

		int index (coo z) const {	if (z.y<center.y)	{ if (z.x<center.x) return 0; else return 1; }
		                         	else             	{ if (z.x<center.x) return 2; else return 3; } }

		void insert (coo z) { ++n; if (leaf) { pts.push_back(z); if (n>m) split(); } else { children[index(z)] -> insert (z); } }

		void split () {
			children[0] = new QuadTree (coo(ul.x,ul.y),        	coo(center.x,center.y),	m, this);
			children[1] = new QuadTree (coo(center.x,ul.y),    	coo(br.x,center.y),    	m, this);
			children[2] = new QuadTree (coo(ul.x,center.y),    	coo(center.x,br.y),    	m, this);
			children[3] = new QuadTree (coo(center.x,center.y),	coo(br.x,br.y),        	m, this);
			for (auto & z : pts) children[index(z)] -> insert (z);
			std::vector<coo>().swap(pts); leaf = false;
		}

		int odist (coo z) { return std::max ({z.x-br.x, z.y-br.y, ul.x-z.x, ul.y-z.y}); }

		void nn (coo z, QuadIndex & qi) const {
			if (leaf) for (auto & w : pts) {
				int newnorm = sup (z-w);
				if (newnorm < qi.d) { qi.d = newnorm; qi.z = w; }
			} else {
				int i0 = index(z); children[i0] -> nn (z,qi);
				for (int i=0; i<4; ++i) if ((i != i0) && (children[i] -> odist(z) < qi.d)) children[i] -> nn (z,qi);
			}
		}

		int dist (coo z) const { QuadIndex qi { 0, sup(z) }; nn (z,qi); return qi.d; }

		coo ul,br,center;
		std::vector <coo> pts;
		QuadTree * children[4], * parent;
		int n,m;
		bool leaf;
	};
}
