#include <vb/LinearAlgebra.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std; using Eigen::Matrix2d;

double lambda (const Matrix2d & A) { return abs (A(0,0) + A(1,1)); }

vector<double> random_p (int k) {
    vector<double> out; double s = 0.0;
    for (int i=0; i<k; ++i) { double ss = prng.uniform_real(); s += ss; out.push_back(ss); }
    for (auto & v : out) v /= s;
    return out;
}

vector<vector<double>> random_markov (int k) {
    vector<vector<double>> p;
    for (int i=0; i<k; ++i) p.push_back (random_p(k));
    return p;
}

class Erwan { public:
    Erwan() {
        Matrix2d A;

        A << 1, +sqrt(3), 0, 1; rho.push_back(A);
        A << 1, -sqrt(3), 0, 1; rho.push_back(A);
        A << 1, 0, +sqrt(3), 1; rho.push_back(A);
        A << 1, 0, -sqrt(3), 1; rho.push_back(A);

        A << 1, +2, 0, 1; Gamma.push_back(A);
        A << 1, -2, 0, 1; Gamma.push_back(A);
        A << 1, 0, +2, 1; Gamma.push_back(A);
        A << 1, 0, -2, 1; Gamma.push_back(A);
    }

    double markov (int n, vector<vector<double>> p) {
        Matrix2d A, B; A << 1, 0, 0, 1; B=A;
        int d=0;
        for (int i=0; i<n; ++i) {
            d = prng.discrete(p[d]);
            A *= rho[d];
            B *= Gamma[d];
        }
        return log(lambda(A)) / log(lambda(B));
    }

    double n2 (int n) {
        return markov (n, {{0.25,0.25,0.25,0.25},{0.25,0.25,0.25,0.25},{0.25,0.25,0.25,0.25},{0.25,0.25,0.25,0.25}});
    }

    double avg (int n, int t, const vector<vector<double>> & p) {
        double s = 0.0;
        for (int i=0; i<t; ++i) s += markov(n,p);
        return s/t;
    }

    vector<vector<double>> explore (int n, int t, int m) {
        double rec = 1.0; vector<vector<double>> best;
        vector<vector<double>> p = random_markov(4);
        for (int i=0; i<m; ++i) {
            p = random_markov(4);
            double tmp = avg (n,t,p);
            if (tmp < rec) {
                H.L->info ("  Current best value: {}", tmp);
                rec = tmp; best = p;
            }
        }
        return best;
    }

    void run (int n, int t) {
        auto p = explore (n,t/100,H['m']);

        for (int i=0; i<4; ++i) {
            H.L->info ("Transition matrix: {} {} {} {}", p[i][0], p[i][1], p[i][2], p[i][3]);
        }

        vector<double> n2s;
        for (int i=0; i<int(t); ++i) n2s.push_back(markov(n,p));
        sort(begin(n2s),end(n2s));
        for (auto v : n2s) cout << v << endl;
        double s=0.0; for (auto v : n2s) s += v; H.L->info ("Average value: {}", s/int(H['t']));
    }

    vector<Matrix2d> rho,Gamma;
};

int main (int argc, char ** argv) {
    H.init ("Erwan's product of random matrices", argc,argv, "n=100,t=100,m=1000");
    Erwan().run(H['n'],H['t']);
}