#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/misc.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Testing PRNG methods");
    int const n = clp.param("n", 100000000, "Number of iterations");
    clp.finalize();

    string const state = prng.state();
    timing("vb::PRNG::gaussian", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });

    timing("boost::gaussian_distribution", [n] {
        boost::normal_distribution<> dist(0, 1);
        boost::mt19937_64            boostengine;
        double                       s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = dist(boostengine);
            s += o * o;
        }
        return s / n;
    });

    timing("many boost::gaussian_distributions", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = (boost::normal_distribution<>(0, 1))(prng);
            s += o * o;
        }
        return s / n;
    });

    timing("C++11 style", [n] {
        std::normal_distribution<> dist2(0, 1);
        std::mt19937_64            engine;
        double                     s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = dist2(engine);
            s += o * o;
        }
        return s / n;
    });

    prng.state(state);
    timing("Stored state, first version", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });
}
