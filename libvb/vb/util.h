#pragma once
#include <vb/Hub.h>
#include <chrono>
#include <iostream>

namespace vb {
    double time();

    template <typename F> void timing(const std::string & label, const F & f) {
        double t      = time();
        auto   result = f();
        H.output_str(label, "", fmt::format("time = {:<10} ans = {}", time() - t, result), false);
    }

    template <typename V>
    std::ostream & vprint(std::ostream & os, const V & v, std::string op = ",", const std::string & in = "(",
                          const std::string & out = ")") {
        std::string sep = "";
        os << in;
        for (const auto & x : v) os << exchange(sep, op) << x;
        return os << out;
    }

    template <typename T> T check(T x, T y) {
        static double merr = -1.0;
        double        err  = abs(x - y);
        if (err > merr) {
            merr = err;
            H.L->info("max error: {}", merr);
        }
        return x;
    }

    template <typename T> std::string pretty(const T & t) { return fmt::format("{}", t); }
} // namespace vb
