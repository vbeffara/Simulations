#pragma once
#include <boost/random.hpp>
#include <random>
#include <vb/util/coo.h>
#include <vector>

namespace vb {
    class PRNG : public boost::mt19937_64 {
    public:
        explicit PRNG(uint64_t s = 0) noexcept : boost::mt19937_64(s ? s : std::random_device()()) {}

        bool                    bernoulli(double p = .5) { return (boost::bernoulli_distribution<>(p))(*this); }
        template <typename T> T uniform_int(T mmax) { return (boost::uniform_int<T>(0, T(mmax - 1)))(*this); }
        int                     geometric(double p = .5) { return (boost::geometric_distribution<>(p))(*this); }
        int                     poisson(double lambda = 1) { return (boost::poisson_distribution<>(lambda))(*this); }
        double                  uniform_real(double min = 0, double max = 1) { return (boost::uniform_real<>(min, max))(*this); }
        double                  exponential(double lambda = 1) { return (boost::exponential_distribution<>(lambda))(*this); }
        double                  gaussian(double m = 0, double sigma = 1) { return (boost::normal_distribution<>(m, sigma))(*this); }

        template <typename T> coo_2d<T> uniform_coo(coo_2d<T> r, T b = 0) {
            return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b)};
        }

        template <typename T> coo_3d<T> uniform_coo3(coo_3d<T> r, T b = 0) {
            return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b), b + uniform_int(r.z - 2 * b)};
        }

        unsigned discrete(const std::vector<double> &p);

        std::string state();
        void        state(const std::string &s);
    };

    extern PRNG prng;
} // namespace vb
