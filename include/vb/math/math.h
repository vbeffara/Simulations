#pragma once
#include <boost/math/constants/constants.hpp>
#include <functional>
#include <vb/util/cpx.h>

namespace vb {
    template <typename T> T sum(const std::function<T(int)> &f) {
        T out(0), old(1);
        for (int n = 0; out != old; ++n) {
            old  = out;
            T dd = f(n);
            if (isnormal(real(dd))) out += dd;
        }
        return out;
    }

    template <typename T> T sign(T x) { return (T(0) < x) - (x < T(0)); }
    template <typename T> T gcd(T a, T b) {
        while (b) {
            auto c = a % b;
            a      = b;
            b      = c;
        }
        return a;
    }
    template <typename T> T lcm(T a, T b) { return a * b / gcd(a, b); }

    template <typename T> T fact(T n) {
        T out(1);
        for (T i(2); i <= n; ++i) out *= i;
        return out;
    }
    template <typename T> T binom(T n, T k) { return fact(n) / fact(k) / fact(n - k); }
    template <typename T> T catalan(T n) { return binom(2 * n, n) / (n + 1); }

    using boost::math::constants::pi;
    template <typename T> T pi_() { return boost::math::constants::pi<T>(); }

    template <typename T> typename cpx_t<T>::type I_() { return {0, 1}; }

    template <typename T> typename cpx_t<T>::type q_(const typename cpx_t<T>::type &tau) {
        return exp(typename cpx_t<T>::type(0, pi_<T>()) * tau);
    }

    template <typename T> typename cpx_t<T>::type q_t(const typename cpx_t<T>::type &tau) {
        return typename cpx_t<T>::type(0, pi_<T>()) * q_<T>(tau);
    }
} // namespace vb
