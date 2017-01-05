#include <vb/Bitmap.h>

using namespace vb; using namespace std;

struct Spin {
	Spin (int s) : s(s) {}
	operator Color() { return 255*s; }
	int s;
};

class Ising : public Bitmap<Spin> { public:
	Ising (int n, double beta, int con) : Bitmap<Spin>(n,n), beta(beta), con(con) {
		for (auto & s : *this) s = prng.bernoulli(.5);
		for (int c=0; c<=con; ++c) p.push_back (1 / (1 + exp(2*beta*(con-2*c))));
	};

	void run() {
		while (true) {
			coo z = rand(); int c = 0;
			for (int d=0; d<con; ++d) c += atp(z+dz[d]).s;
			put(z,prng.bernoulli(p[c]));
		}
	}

	double beta;
	int con;
	vector<double> p;
};

int main (int argc, char ** argv) {
	H.init ("2D Ising model", argc, argv, "n=500,b=.7,c=4");
	Ising I (H['n'],H['b'],H['c']); I.show(); I.run();
}
