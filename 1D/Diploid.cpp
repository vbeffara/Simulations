#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

class Diploid { public:
	int K;
	double f, d, delta, GC, eta;

	Diploid () : K(H['K']), f(H['f']), d(H['d']), delta(H['D']), GC(H['c']), eta(H['e']) {
		double DaA = d, DAA = d, Daa = d + delta, DaB = d - delta, DAB = d - delta, DBB = d - delta;
		DR = {Daa, DaA, DAA, DaB, DAB, DBB};
		B = {0,0,0,0,0,0}; De = {0,0,0,0,0,0};

		double c0 = GC/K, c1 = GC/K, c2 = GC/K, c3 = (GC - eta)/K;
		Comp = {{c0,c1,c2,0,0,0},
				{c1,c0,c1,c1,c2,c3},
				{c2,c1,c0,c2,c1,c2},
				{0,c1,c2,c0,c1,c2},
				{0,c2,c1,c1,c0,c1},
				{0,c3,c2,c2,c1,c0}};

		nu = { int(K*double(H['x'])), int(K*double(H['y'])), int(K*double(H['z'])), 0, 1, 0 };
	}

	void compute_B () {
		const double naa = nu[0], naA = nu[1], nAA = nu[2], naB = nu[3], nAB = nu[4], nBB = nu[5];

		B[0] = naa * (naa + naA/2) / (naa + naA + nAA)
			+ (naA/2 + naB/2) * naB / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naa + naA/2 + naB/2) / (2 * (naa + naA + nAA + naB + nAB + nBB));
		B[1] = .25 * (2 * naa * (naA + 2 * nAA) / (naa + naA + nAA)
			+ (2 * naB * (nAA + nAB) + naA * (naB + nAB)) / (naA + nAA + naB + nAB + nBB)
			+ ((naA + 2 * nAA) * (2 * (naa + naA) + naB) + naA * nAB) / (naa + naA + nAA + naB + nAB + nBB));
		B[2] = (naA/2 + nAA + nAB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naA/2 + nAA + nAB/2) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ nAA * (naA/2 + nAA + nAB/2) / (naa + naA + nAA + naB + nAB + nBB);
		B[3] = (naA/2 + naB/2) * naB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + naB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + naB/2) * nBB / (naA + nAA + naB + nAB + nBB)
			+ (naB * (naB/2 + nAB/2 + nBB)) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naB/2 + nAB/2 + nBB)) / (2 * (naa + naA + nAA + naB + nAB + nBB));
		B[4] = naB * (naA/2 + nAA + nAB/2) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + nAA + nAB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + nAA + nAB/2) * nBB / (naA + nAA + naB + nAB + nBB)
			+ nAB * (naB/2 + nAB/2 + nBB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naB/2 + nAB/2 + nBB) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ nAA * (naB/2 + nAB/2 + nBB) / (naa + naA + nAA + naB + nAB + nBB);
		B[5] = (naB + nAB + 2 * nBB) * (naB + nAB + 2 * nBB) / (4 * (naA + nAA + naB + nAB + nBB));
	}

	double NewNu () {
		for (int i=0; i<6; ++i) {
			De[i] = DR[i];
			for (int j=0; j<6; ++j) De[i] += Comp[i][j] * nu[j];
			De[i] *= nu[i];
		}

		compute_B();

		double BB = 0; for (const auto & x : B) BB += x;
		double DD = 0; for (const auto & x : De) DD += x;

		if (prng.bernoulli (BB*f/(BB*f+DD))) {
			for (auto & x : B) x /= BB;
			nu[prng.discrete(B)] += 1;
		} else {
			for (auto & x : De) x /= DD;
		    nu[prng.discrete(De)] -= 1;
		}

		return prng.exponential(BB*f+DD);
	}

	vector<int> nu;
	vector<double> DR, B, De;
	vector<vector<double>> Comp;
};

int main (int argc, char ** argv) {
	H.init ("Diploid model", argc, argv, "s=0,i=60000,K=200,f=2,d=.5,D=.5,c=1,e=.003,x=.1,y=.3,z=1.5,t=1");

	if (int seed = H['s']) prng.seed(seed);

	Diploid D;

	double t = 0; int iterations = H['i'], period = H['t'];
	for (int k=0; k<iterations; ++k) {
		t += D.NewNu();
		if (!(k%period)) { cout << t; for (const auto & x : D.nu) cout << " " << x; cout << endl; }
	}

	cerr << endl << R"(plot 'out' using 1:2 w l lt rgb "blue" title "naa", 'out' using 1:3 w l lt rgb "green" title "naA", 'out' using 1:4 w l lt rgb "red" title "nAA", 'out' using 1:5 w l lt rgb "purple" title "naB", 'out' using 1:6 w l lt rgb "orange" title "nAB", 'out' using 1:7 w l lt rgb "black" title "nBB")" << endl << endl;
	cerr << t; for (const auto & x : D.nu) cerr << " " << x; cerr << endl;
}
