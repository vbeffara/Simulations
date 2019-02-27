#include <gsl/gsl>
#include <vb/LinearAlgebra.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;
using Eigen::Matrix2d;

#include <cmaes.h>
using namespace libcmaes;

double lambda(const Matrix2d &A) { return abs(A(0, 0) + A(1, 1)); }

vector<double> random_p(int k) {
    vector<double> out;
    double         s = 0.0;
    for (int i = 0; i < k; ++i) {
        double ss = prng.uniform_real();
        s += ss;
        out.push_back(ss);
    }
    for (auto &v : out) v /= s;
    return out;
}

vector<vector<double>> random_markov(int k) {
    vector<vector<double>> p(k);
    for (auto &x : p) x = random_p(k);
    return p;
}

class Erwan {
public:
    Erwan() {
        Matrix2d A;

        A << 1, +sqrt(3), 0, 1;
        rho.push_back(A);
        A << 1, -sqrt(3), 0, 1;
        rho.push_back(A);
        A << 1, 0, +sqrt(3), 1;
        rho.push_back(A);
        A << 1, 0, -sqrt(3), 1;
        rho.push_back(A);

        A << 1, +2, 0, 1;
        Gamma.push_back(A);
        A << 1, -2, 0, 1;
        Gamma.push_back(A);
        A << 1, 0, +2, 1;
        Gamma.push_back(A);
        A << 1, 0, -2, 1;
        Gamma.push_back(A);
    }

    double markov(int n, vector<vector<double>> p) {
        Matrix2d A, B;
        A << 1, 0, 0, 1;
        B     = A;
        int d = 0;
        for (int i = 0; i < n; ++i) {
            d = prng.discrete(p[d]);
            A *= rho[d];
            B *= Gamma[d];
        }
        return log(lambda(A)) / log(lambda(B));
    }

    double n2(int n) {
        return markov(n, {{0.25, 0.25, 0.25, 0.25}, {0.25, 0.25, 0.25, 0.25}, {0.25, 0.25, 0.25, 0.25}, {0.25, 0.25, 0.25, 0.25}});
    }

    double avg(int n, int t, const vector<vector<double>> &p) {
        double s = 0.0;
        for (int i = 0; i < t; ++i) s += markov(n, p);
        return s / t;
    }

    vector<vector<double>> x_to_p(const gsl::span<const double> x) {
        vector<vector<double>> p;
        for (int i = 0; i < 4; ++i) {
            p.emplace_back();
            double s = 0;
            for (int j = 0; j < 4; ++j) {
                double ss = x[4 * i + j] * x[4 * i + j];
                if (ss < .2) ss = .2;
                if (ss > 5) ss = 5;
                p.back().push_back(ss);
                s += ss;
            }
            for (auto &v : p.back()) v /= s;
        }
        return p;
    }

    vector<vector<double>> explore_cmaes(int n, int t) {
        FitFunc         f = [this, n, t](const double *x, const int /* N */) { return abs(avg(n, t, x_to_p({x, 16})) - 2.0 / 3.0); };
        vector<double>  x0(16, .25);
        double          sigma = 0.1;
        CMAParameters<> cmaparams(x0, sigma);
        CMASolutions    cmasols = cmaes<>(f, cmaparams);
        return x_to_p({cmasols.get_best_seen_candidate().get_x_ptr(), 16});
    }

    void run(const Hub &H, int n, int t) {
        auto p = explore_cmaes(n, t / 1000);

        for (int i = 0; i < 4; ++i) { spdlog::info("Transition matrix: {} {} {} {}", p[i][0], p[i][1], p[i][2], p[i][3]); }

        vector<double> n2s(int{t});
        for (auto &x : n2s) x = markov(n, p);
        sort(begin(n2s), end(n2s));
        for (auto v : n2s) cout << v << endl;
        double s = 0.0;
        for (auto v : n2s) s += v;
        spdlog::info("Average value: {}", s / int(H['t']));
    }

    vector<Matrix2d> rho, Gamma;
};

int main(int argc, char **argv) {
    Hub H("Erwan's product of random matrices", argc, argv, "n=1000,t=10000");
    Erwan().run(H, H['n'], H['t']);
}
