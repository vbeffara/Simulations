#pragma once
#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>

namespace vb {
    double time();

    template <typename F> void timing(Hub &H, const std::string &label, const F &f) {
        double t      = time();
        auto   result = f();
        H.output(label, "", fmt::format("time = {:>7.3f} ans = {}", time() - t, result), false);
    }

    template <typename T> T check(T x, T y) {
        static double merr = -1.0;
        double        err  = abs(x - y);
        if (err > merr) {
            merr = err;
            spdlog::info("max error: {}", merr);
        }
        return x;
    }
} // namespace vb
