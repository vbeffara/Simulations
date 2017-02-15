#include <vb/CoarseImage.h>
#include <vb/Point.h>

using namespace vb;

double cost () { return prng.exponential(); }

class FPP : public CoarseImage {
public:
	FPP (int n) : CoarseImage (n,n, pow(n,.33)), area(0) {
		invasion = H['i']; twostep = H['2']; trace = H['t'];

		pq.push ({{n>>1,n>>1},cost()});
		if (twostep) pq.push ({{(true_width>>1)+1, true_height>>1}, cost()});
	};

	void spread (double t, coo z) {
		for (int d=0; d<4; ++d) if (auto zz=z+dz[d]; !at(zz)) pq.push ({zz,t+cost()});
	}

	void run () {
		while (true) {
			if (trace) std::cout << area << " " << pq.size() << std::endl;

			if (auto pt=pq.get(); !at(pt)) {
				put(pt,1); ++area;

				double curtime = invasion ? 0.0 : pt.t;

				int deg=1; if (twostep) for (int d=0; d<4; ++d) deg += at(pt+dz[d]);

				for (int i=0; i<deg; ++i) spread (curtime,pt);
				if (twostep) for (int d=0; d<4; ++d) if (auto zz=pt+dz[d]; at(zz)) spread (curtime,zz);

				if ( (pt.x==1) || (pt.y==1) || (pt.x==true_width-2) || (pt.y==true_height-2) ) break;
			}
		}
	}

	int area;
	bool trace, invasion, twostep;
	PointQueue pq;
};

int main (int argc, char **argv) {
	H.init ("First-passage percolation", argc,argv, "n=5000,i,2,t");
	FPP F (H['n']); F.show(); F.run(); F.output();
}
