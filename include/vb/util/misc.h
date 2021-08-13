#pragma once
#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>

namespace vb {
    auto time() -> double;

    template <typename F> void timing(Hub &H, const std::string &label, const F &f) {
        double t      = time();
        auto   result = f();
        H.output(label, "", fmt::format("time = {:>7.3f} ans = {}", time() - t, result), false);
    }

    template <typename T> auto check(T x, T y) -> T {
        static double merr = -1.0;
        double        err  = abs(x - y);
        if (err > merr) {
            merr = err;
            spdlog::info("max error: {}", merr);
        }
        return x;
    }

    template <typename T> auto to_signed(T x) -> std::enable_if_t<std::is_unsigned_v<T>, std::make_signed_t<T>> {
        return static_cast<std::make_signed_t<T>>(x);
    }

    template <typename T> auto to_unsigned(T x) -> std::enable_if_t<std::is_signed_v<T>, std::make_unsigned_t<T>> {
        assert(x >= 0); // NOLINT
        return static_cast<std::make_unsigned_t<T>>(x);
    }
} // namespace vb
