#pragma once
#include <vb/Hub.h>
#include <boost/lexical_cast.hpp>
#include <chrono>
#include <iostream>

namespace vb {
    template <typename T> auto str(const T & t) { return boost::lexical_cast<std::string>(t); }

    double time();

    template <typename F> void timing(const std::string & label, const F & f) {
        double t      = time();
        auto   result = f();
        H.L->info("{:<60} time = {:<10} ans = {}", label, time() - t, result);
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
} // namespace vb
