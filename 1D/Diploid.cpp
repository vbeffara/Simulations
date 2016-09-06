#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

vector<double> BR (const vector<int> & nu) {
	const double naa = nu[0], naA = nu[1], nAA = nu[2], naB = nu[3], nAB = nu[4], nBB = nu[5];
	return {
		naa * (naa + naA/2) / (naa + naA + nAA)
			+ (naA/2 + naB/2) * naB / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naa + naA/2 + naB/2) / (2 * (naa + naA + nAA + naB + nAB + nBB)),
	    .25 * (2 * naa * (naA + 2 * nAA) / (naa + naA + nAA)
			+ (2 * naB * (nAA + nAB) + naA * (naB + nAB)) / (naA + nAA + naB + nAB + nBB)
			+ ((naA + 2 * nAA) * (2 * (naa + naA) + naB) + naA * nAB) / (naa + naA + nAA + naB + nAB + nBB)),
		(naA/2 + nAA + nAB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naA/2 + nAA + nAB/2) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ nAA * (naA/2 + nAA + nAB/2) / (naa + naA + nAA + naB + nAB + nBB),
		(naA/2 + naB/2) * naB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + naB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + naB/2) * nBB / (naA + nAA + naB + nAB + nBB)
			+ (naB * (naB/2 + nAB/2 + nBB)) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naB/2 + nAB/2 + nBB)) / (2 * (naa + naA + nAA + naB + nAB + nBB)),
		naB * (naA/2 + nAA + nAB/2) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + nAA + nAB/2) * nAB / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA/2 + nAA + nAB/2) * nBB / (naA + nAA + naB + nAB + nBB)
			+ nAB * (naB/2 + nAB/2 + nBB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ naA * (naB/2 + nAB/2 + nBB) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ nAA * (naB/2 + nAB/2 + nBB) / (naa + naA + nAA + naB + nAB + nBB),
		(naB + nAB + 2 * nBB) * (naB + nAB + 2 * nBB) / (4 * (naA + nAA + naB + nAB + nBB))
	};
}

double f = 2, d = .5, delta = .5, GC = 1, eta = .003;
int K = 200;

double DaA = d, DAA = d, Daa = d + delta, DaB = d - delta, DAB = d - delta, DBB = d - delta;
vector<double> DR = {Daa, DaA, DAA, DaB, DAB, DBB};

double c0 = GC/K, c1 = GC/K, c2 = GC/K, c3 = (GC - eta)/K, c4 = (GC - eta)/K;

vector<vector<double>> Comp =
	{{c0, c1, c2, 0, 0, 0},
	{c1, c0, c1, c1, c2, c3},
	{c2, c1, c0, c2, c1, c2},
	{0, c1, c2, c0, c1, c2},
	{0, c2, c1, c1, c0, c1},
	{0, c3, c2, c2, c1, c0}};

double NewNu (vector<int> & nu) {
	vector<double> De = {0,0,0,0,0,0};

	for (int i=0; i<6; ++i) {
		De[i] = DR[i];
		for (int j=0; j<6; ++j) De[i] += Comp[i][j] * nu[j];
		De[i] *= nu[i];
	}

	vector<double> B = BR(nu); for (auto & x : B) x *= f;

	double BB = 0; for (auto x : B) BB += x;
	double DD = 0; for (auto x : De) DD += x;
	double Tot = BB + DD;

	if (prng.bernoulli (BB/Tot)) {
		for (auto & x : B) x /= BB;
		int x = prng.discrete(B);
		nu[x] += 1;
	} else {
		for (auto & x : De) x /= DD;
		int x = prng.discrete(De);
	    nu[x] -= 1; if (nu[x]<.1) nu[x] = 0;
	}

	return prng.exponential(Tot);
}

int main (int argc, char ** argv) {
	H.init ("Diploid model", argc, argv, "s=0,i=60000");

	if (int seed = H['s']) prng.seed(seed);
	int iterations = H['i'];

	vector<int> nu = { K/5, K, 5*K, 0, 1, 0};

	double t = 0;

	for (int k=1; k<iterations; ++k) {
		t += NewNu (nu);
		cout << t; for (const auto & x : nu) cout << " " << x; cout << endl;
	}

	cerr << t; for (const auto & x : nu) cerr << " " << x; cerr << endl;
}
