#pragma once
#include <boost/math/constants/constants.hpp>
#include <functional>
#include <vb/util/cpx.h>

namespace vb {
    template <typename T> auto sum(const std::function<T(int)> &f) -> T {
        T out(0), old(1);
        for (int n = 0; out != old; ++n) {
            old  = out;
            T dd = f(n);
            using std::isnormal;
            if (isnormal(real(dd))) out += dd;
        }
        return out;
    }

    template <typename T> auto sign(T x) -> T { return (T(0) < x) - (x < T(0)); }

    template <typename T> auto fact(T n) -> T {
        T out(1);
        for (T i(2); i <= n; ++i) out *= i;
        return out;
    }
    template <typename T> auto binom(T n, T k) -> T { return fact(n) / fact(k) / fact(n - k); }
    template <typename T> auto catalan(T n) -> T { return binom(2 * n, n) / (n + 1); }

    template <typename T> auto pi_() -> T { return boost::math::constants::pi<T>(); }

    template <typename T> auto I_() -> typename cpx_t<T>::type { return {0, 1}; }

    template <typename T> auto q_(const typename cpx_t<T>::type &tau) -> typename cpx_t<T>::type {
        return exp(typename cpx_t<T>::type(0, pi_<T>()) * tau);
    }

    template <typename T> auto q_t(const typename cpx_t<T>::type &tau) -> typename cpx_t<T>::type {
        return typename cpx_t<T>::type(0, pi_<T>()) * q_<T>(tau);
    }
} // namespace vb
