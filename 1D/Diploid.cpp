#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

double f = 2;
double d = .5;
double delta = .5;
double GC = 1;
double eta = .003;

vector<double> BR (vector<double> nu) {
	assert (nu.size()==6);
	double naa = nu[0], naA = nu[1], nAA = nu[2], naB = nu[3], nAB = nu[4], nBB = nu[5];
	return {
		(naa * (naa + naA/2)) / (naa + naA + nAA)
			+ ((naA/2 + naB/2) * naB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naa + naA/2 + naB/2)) / (2 * (naa + naA + nAA + naB + nAB + nBB)),
	    .25 * ((2 * naa * (naA + 2 * nAA)) / (naa + naA + nAA)
			+ (2 * naB * (nAA + nAB) + naA * (naB + nAB)) / (naA + nAA + naB + nAB + nBB)
			+ ((naA + 2 * nAA) * (2 * (naa + naA) + naB) + naA * nAB) / (naa + naA + nAA + naB + nAB + nBB)),
		((naA/2 + nAA + nAB/2) * nAB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naA/2 + nAA + nAB/2)) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ (nAA * (naA/2 + nAA + nAB/2)) / (naa + naA + nAA + naB + nAB + nBB),
		((naA/2 + naB/2) * naB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ ((naA/2 + naB/2) * nAB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ ((naA/2 + naB/2) * nBB) / (naA + nAA + naB + nAB + nBB)
			+ (naB * (naB/2 + nAB/2 + nBB)) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naB/2 + nAB/2 + nBB)) / (2 * (naa + naA + nAA + naB + nAB + nBB)),
		(naB * (naA/2 + nAA + nAB/2)) / (2 * (naA + nAA + naB + nAB + nBB))
			+ ((naA/2 + nAA + nAB/2) * nAB) / (2 * (naA + nAA + naB + nAB + nBB))
			+ ((naA/2 + nAA + nAB/2) * nBB) / (naA + nAA + naB + nAB + nBB)
			+ (nAB * (naB/2 + nAB/2 + nBB)) / (2 * (naA + nAA + naB + nAB + nBB))
			+ (naA * (naB/2 + nAB/2 + nBB)) / (2 * (naa + naA + nAA + naB + nAB + nBB))
			+ (nAA * (naB/2 + nAB/2 + nBB)) / (naa + naA + nAA + naB + nAB + nBB),
		pow ((naB + nAB + 2 * nBB),2) / (4 * (naA + nAA + naB + nAB + nBB))
	};
}

double DaA = d, DAA = d;
double Daa = d + delta;
double DaB = d - delta, DAB = d - delta, DBB = d - delta;

vector<double> DR = {Daa, DaA, DAA, DaB, DAB, DBB};

double c0 = GC;
double c1 = GC;
double c2 = GC;
double c3 = GC - eta;
double c4 = GC - eta;

vector<vector<double>> Comp = {
   {c0, c1, c2, 0, 0, 0},
   {c1, c0, c1, c1, c2, c3},
   {c2, c1, c0, c2, c1, c2},
   {0, c1, c2, c0, c1, c2},
   {0, c2, c1, c1, c0, c1},
   {0, c3, c2, c2, c1, c0}};

pair<double,vector<double>> NewNu (vector<double> nu, double K) {
	double BB = 0;
	vector<double> De = {0,0,0,0,0,0}; double DD = 0;
	vector<double> Inc = {0,0,0,0,0,0}; double Tot = 0;

	vector<double> B = BR(nu); for (auto & x : B) x *= K*f;

	for (int i=0; i<6; ++i) {
		for (int j=0; j<6; ++j) De[i] += Comp[i][j] * nu[j];
		De[i] += DR[i];
		De[i] *= K*nu[i];
	}

	for (auto x : B) BB += x;
	for (auto x : De) DD += x;

	Tot = BB + DD;
	double u = prng.uniform_real();

	if (u < BB/(BB + DD)) {
		for (auto & x : B) x /= BB;
		int x = prng.discrete(B);
		Inc[x] = 1/K;
	} else {
		for (auto & x : De) x /= DD;
		int x = prng.discrete(De);
	    Inc[x] = -1/K;
	}

	for (int i=0; i<6; ++i) { nu[i] = nu[i]+Inc[i]; if (nu[i] < .5/K) nu[i]=0; }
	return {Tot, nu};
}

int main (int argc, char ** argv) {
	H.init ("Diploid model", argc, argv, "s=0");

	if (int seed = H['s']) prng.seed(seed);

	double K = 200;
	vector<double> nu = {.2, 1, 5, 0, 1/K, 0};
	double t = 0, s = 0;
	int iterations = 60000;

	vector<double> time (iterations, 0);
	vector<vector<double>> nut (iterations, {0,0,0,0,0,0});
	nut[0] = nu;

	for (int k=1; k<iterations; ++k) {
		auto nn = NewNu (nu,K);
		double Tot = nn.first; nu = nn.second;
		if (Tot==0) { cerr << "DEAD" << endl; break; }

		nut[k] = nu;
		s = prng.exponential (Tot);
		t += s;
		time[k] = t;

		cout << t; for (auto x : nu) cout << " " << x; cout << endl;
	}

	cerr << t; for (auto x : nu) cerr << " " << x; cerr << endl;
}
