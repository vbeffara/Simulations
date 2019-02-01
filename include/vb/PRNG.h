#pragma once
#define BOOST_DISABLE_ASSERTS 1
namespace std {
    template <typename T1, typename T2> auto bind2nd(T1 t1, T2 t2) {
        return [t1, t2](auto t3) { return t1(t3, t2); };
    }
} // namespace std
#include <boost/random.hpp>
#include <random>
#include <vb/util/coo.h>
#include <vector>

namespace vb {
    class PRNG : public boost::mt19937_64 {
    public:
        explicit PRNG(uint64_t s = 0) : boost::mt19937_64(s ? s : std::random_device()()) {}

        bool   bernoulli(double p = .5) { return (boost::bernoulli_distribution<>(p))(*this); }
        int    uniform_int(int mmax) { return (boost::uniform_int<>(0, mmax - 1))(*this); }
        int    geometric(double p = .5) { return (boost::geometric_distribution<>(p))(*this); }
        int    poisson(double lambda = 1) { return (boost::poisson_distribution<>(lambda))(*this); }
        double uniform_real(double min = 0, double max = 1) { return (boost::uniform_real<>(min, max))(*this); }
        double exponential(double lambda = 1) { return (boost::exponential_distribution<>(lambda))(*this); }
        double gaussian(double m = 0, double sigma = 1) { return (boost::normal_distribution<>(m, sigma))(*this); }
        coo    uniform_coo(coo r, int64_t b = 0) { return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b)}; }
        coo3   uniform_coo3(coo3 r, int64_t b = 0) {
            return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b), b + uniform_int(r.z - 2 * b)};
        }

        int discrete(const std::vector<double> &p);

        std::string state();
        void        state(const std::string &s);
    };

    extern PRNG prng;
} // namespace vb
