#include <vb/CoarseImage.h>
#include <vb/Point.h>

using namespace vb;

double cost () { return prng.exponential(); }

class FPP : public CoarseImage {
public:
	explicit FPP (int n) : CoarseImage (n,n, pow(n,.33)), area(0) {
		invasion = H['i']; twostep = H['2']; trace = H['t'];

		pq.push ({{n>>1,n>>1},cost()});
		if (twostep) pq.push ({{(true_width>>1)+1, true_height>>1}, cost()});
	};

	void spread (double t, const coo &z) {
		for (int d=0; d<4; ++d) { auto zz=z+dz[d]; if (!at(zz)) pq.push ({zz,t+cost()}); }
	}

	void run () {
		while (true) {
			if (trace) std::cout << area << " " << pq.size() << std::endl;

			auto pt = pq.get(); const coo &z = pt;
			if (!at(z)) {
				put(z,1); ++area;

				double curtime = invasion ? 0.0 : pt.t;

				int deg=1; if (twostep) for (int d=0; d<4; ++d) deg += static_cast<int>(at(z+dz[d]));

				for (int i=0; i<deg; ++i) spread (curtime,z);
				if (twostep) for (int d=0; d<4; ++d) { auto zz=z+dz[d]; if (at(zz)) spread (curtime,zz); }

				if ( (z.x==1) || (z.y==1) || (z.x==true_width-2) || (z.y==true_height-2) ) break;
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
