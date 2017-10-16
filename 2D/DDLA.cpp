#include <vb/CoarseImage.h>
#include <vb/Point.h>

using namespace vb; using namespace std;

class DDLA : public CoarseImage {
public:
	explicit DDLA (int n_) : CoarseImage (n_, n_, H['f'] ? pow(n_,.33) : 1), f(H['f']), n(n_), cursum(0), p(H['p']) {
		put({0,0},1);
		pq.push ({{1,0},prng.exponential()/p}); pq.push ({{0,1},prng.exponential()/(1-p)});
	};

	bool ok (coo z) {
		while (true) {
			if ( (z.x >= n) || (z.y >= n) || (z.x+z.y > cursum) ) return true;
			if (at(z)) return false;
			z += dz[static_cast<std::__1::vector<class vb::coo, class std::__1::allocator<class vb::coo> >::size_type>(prng.bernoulli(p))];
		}
	}

	void fill (coo z) {
		if ( (z.x > 0) && (z.y < n-1) && at(z+coo{-1,1}) && !at(z-coo{1,0}) ) { put(z-coo{1,0},1); fill (z-coo{1,0}); }
		if ( (z.y > 0) && (z.x < n-1) && at(z+coo{1,-1}) && !at(z-coo{0,1}) ) { put(z-coo{0,1},1); fill (z-coo{0,1}); }
	}

	void run () {
		while (true) {
			const Point pt = pq.get(); const coo &z = pt;

			if ((z.x == n-1) || (z.y == n-1)) return;
			if (!at(z)) {
				double curtime = pt.t;
				if (ok(z)) {
					put(z,1); cursum = max (cursum, z.x+z.y); if (f) fill (z);
					if ( (z.x<n-1) && (static_cast<int>(at(z+coo{1,0}))==0) ) pq.push ({z+coo{1,0},curtime+prng.exponential()/p});
					if ( (z.y<n-1) && (static_cast<int>(at(z+coo{0,1}))==0) ) pq.push ({z+coo{0,1},curtime+prng.exponential()/(1-p)});
				} else {
					pq.push ({z,curtime+prng.exponential()});
				}
			}
		}
	}

	bool f;
	int64_t n,cursum;
	double p;
	PointQueue pq;
};

int main (int argc, char ** argv) {
	H.init ("Directed DLA",argc,argv,"n=750,p=.5,f");
	DDLA img (H['n']); img.show(); img.run(); img.output();
}
