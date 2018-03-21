#include <vb/Ranges.h>

using namespace vb;
using namespace std;

int main(int argc, char ** argv) {
    H.init("Testing PRNG methods", argc, argv, "n=100000000");
    int n = H['n'];

    string state = prng.state();
    timing("vb::PRNG::gaussian", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });

    timing("vb::PRNG::gaussian through range", [n] {
        auto os = rv::generate([] { return prng.gaussian(); }) | rv::transform([](double o) { return o * o; }) | rv::take(n);
        return ranges::accumulate(os, 0.0) / n;
    });

    timing("boost::gaussian_distribution", [n] {
        boost::normal_distribution<> dist(0, 1);
        boost::mt19937_64            boostengine;
        double                       s = 0;
        for (int i = 0; i < n; ++i) {
            double o = dist(boostengine);
            s += o * o;
        }
        return s / n;
    });

    timing("many boost::gaussian_distributions", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double o = (boost::normal_distribution<>(0, 1))(prng);
            s += o * o;
        }
        return s / n;
    });

    timing("C++11 style", [n] {
        std::normal_distribution<> dist2(0, 1);
        std::mt19937_64            engine;
        double                     s = 0;
        for (int i = 0; i < n; ++i) {
            double o = dist2(engine);
            s += o * o;
        }
        return s / n;
    });

    prng.state(state);
    timing("Stored state, first version", [n] {
        double s = 0;
        for (int i = 0; i < n; ++i) {
            double o = prng.gaussian();
            s += o * o;
        }
        return s / n;
    });

#ifdef _OPENMP
    timing("OpenMP based multithreaded", [n] {
        auto go = [] {
            static thread_local PRNG p;
            auto                     o = p.gaussian();
            return o * o;
        };
        double s = 0;
#pragma omp parallel for reduction(+ : s)
        for (int i = 0; i < n; ++i) s += go();
        return s / n;
    });
#endif
}
