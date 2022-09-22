#include <vb/util/PRNG.h>
#include <vb/util/misc.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    Hub H("Testing PRNG methods", argc, argv, "n=100000000");
    int const n = H['n'];

    string const state = prng.state();
    timing(H, "vb::PRNG::gaussian", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });

    timing(H, "boost::gaussian_distribution", [n] {
        boost::normal_distribution<> dist(0, 1);
        boost::mt19937_64            boostengine;
        double                       s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = dist(boostengine);
            s += o * o;
        }
        return s / n;
    });

    timing(H, "many boost::gaussian_distributions", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = (boost::normal_distribution<>(0, 1))(prng);
            s += o * o;
        }
        return s / n;
    });

    timing(H, "C++11 style", [n] {
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
    timing(H, "Stored state, first version", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double const o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });
}
