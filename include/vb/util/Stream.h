#pragma once
#define BOOST_CONTEXT_DETAIL_APPLY_H
#include <boost/coroutine2/all.hpp>

namespace vb {
    template <typename T> using Stream = boost::coroutines2::detail::pull_coroutine<T>;
    template <typename T> using Sink   = boost::coroutines2::detail::push_coroutine<T>;

    template <typename T> int size(Stream<T> &&S) {
        int out = 0;
        for (const auto &i : S) {
            (void)i;
            out++;
        }
        return out;
    }

    template <typename T> auto take(int n, Stream<T> &&S) {
        return Stream<T>([n, S = std::move(S)](Sink<T> &yield) mutable {
            if (n <= 0) return;
            for (const auto &x : S) {
                yield(x);
                --n;
                if (n == 0) break;
            }
        });
    }

    template <typename F, typename T> auto filter(F f, Stream<T> &&S) {
        return Stream<T>([f, S = std::move(S)](Sink<T> &yield) mutable {
            for (const auto &x : S)
                if (f(x)) yield(x);
        });
    }

    template <typename F, typename T> auto fmap(F f, Stream<T> &&S) {
        return Stream<decltype(f(T()))>([f, S = std::move(S)](Sink<decltype(f(T()))> &yield) mutable {
            for (const auto &x : S) yield(f(x));
        });
    }
} // namespace vb
