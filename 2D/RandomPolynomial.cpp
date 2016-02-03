#include <vb/Coloring.h>
#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <vb/math.h>

using namespace vb; using namespace std;

template <typename T> T sm3 (int i, int j, int k) {
	static T np = 9 * pi_<T>();
	if (i<j) return sm3<T> (j,i,k);
	if (i<k) return sm3<T> (k,i,j);
	if (i==0) return 1.0;
	return sqrt(T(i+j+k)/(np*T(i))) * sm3<T> (i-1,j,k);
}

template <typename T> class RPoly { public:
	RPoly (int n_) : n(n_), A(n+1) {
		map<string,function<double()>> generators;
		generators.emplace ("gaussian",  [](){ return prng.gaussian(0,1); });
		generators.emplace ("bernoulli", [](){ return prng.bernoulli(.5) ? 1 : -1; });

		for (int i=0; i<=n; ++i)
			for (int j=0; j<=n-i; ++j) {
				T a (generators[H['g']]());
				T b = a * sm3<T> (i,j,n-i-j);
				A[i].push_back(b);
			}
	}

	T operator() (T x, T y) {
		T out = A[n][0];
		for (int i=n-1; i>=0; --i) {
			T pi = A[i][n-i];
			for (int j=n-i-1; j>=0; --j) pi = y*pi + A[i][j];
			out = x*out + pi;
		}
		return out;
	}

	Color operator() (cpx z) { return HSV ((*this)(T(real(z)),T(imag(z)))>0 ? 0 : .5, .8, .8); }

	int n;
	vector<vector<T>> A;
};

int main (int argc, char ** argv) {
	H.init ("Random polynomial in 2 variables", argc, argv, "n=100,g=gaussian,s=0");
	int s = H['s']; if (s) prng.seed(s);
	RPoly<double> P (H['n']); Coloring C (cpx(-10,-10),cpx(10,10),800,P);
	C.show(); C.output(); Fl::run();
}
