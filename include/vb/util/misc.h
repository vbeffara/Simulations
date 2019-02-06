#pragma once
#include <chrono>
#include <vb/util/Hub.h>

namespace vb {
    double time();

    template <typename F> void timing(const std::string &label, const F &f) {
        double t      = time();
        auto   result = f();
        H.output_str(label, "", fmt::format("time = {:<10} ans = {}", time() - t, result), false);
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
