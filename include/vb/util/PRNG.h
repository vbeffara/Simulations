#pragma once
#include <boost/random.hpp>
#include <random>
#include <vb/util/coo.h>
#include <vector>

namespace vb {
  class PRNG : public boost::mt19937_64 {
public:
    explicit PRNG(uint64_t s = 0) noexcept : boost::mt19937_64(s != 0U ? s : std::random_device()()) {}

    auto                       bernoulli(double p = .5) -> bool { return (boost::bernoulli_distribution<>(p))(*this); }
    template <typename T> auto uniform_int(T mmax) -> T { return (boost::uniform_int<T>(0, T(mmax - 1)))(*this); }
    auto                       geometric(double p = .5) -> int { return (boost::geometric_distribution<>(p))(*this); }
    auto                       poisson(double lambda = 1) -> int { return (boost::poisson_distribution<>(lambda))(*this); }
    auto                       uniform_real(double min = 0, double max = 1) -> double { return (boost::uniform_real<>(min, max))(*this); }
    auto                       exponential(double lambda = 1) -> double { return (boost::exponential_distribution<>(lambda))(*this); }
    auto gaussian(double m = 0, double sigma = 1) -> double { return (boost::normal_distribution<>(m, sigma))(*this); }

    template <typename T> auto uniform_coo(coo_2d<T> r, T b = 0) -> coo_2d<T> {
      return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b)};
    }

    template <typename T> auto uniform_coo3(coo_3d<T> r, T b = 0) -> coo_3d<T> {
      return {b + uniform_int(r.x - 2 * b), b + uniform_int(r.y - 2 * b), b + uniform_int(r.z - 2 * b)};
    }

    auto discrete(const std::vector<double> &p) -> unsigned;

    auto state() -> std::string;
    void state(const std::string &s);
  };

  extern thread_local PRNG prng;
} // namespace vb

template <> struct fmt::formatter<vb::PRNG> : ostream_formatter {}; // TODO: do this better
