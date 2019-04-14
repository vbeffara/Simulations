#pragma once
#include <boost/coroutine2/all.hpp>

namespace vb {
    template <typename T> using Stream = boost::coroutines2::detail::pull_coroutine<T>;
    template <typename T> using Sink   = boost::coroutines2::detail::push_coroutine<T>;

    Stream<std::vector<size_t>> partitions(size_t n, size_t m = 1); // Partitions of n with piece size at least m
    Stream<std::vector<size_t>> tuples(size_t k, size_t n);         // Ordered, distinct k-tuples in [0,n-1]
    Stream<std::vector<size_t>> cycles(size_t k, size_t n);         // k-tuples up to cyclic permutation

    template <typename T> size_t size(Stream<T> &&S) {
        size_t out = 0;
        for (const auto &i : S) {
            (void)i;
            out++;
        }
        return out;
    }

    template <typename T> auto take(size_t n, Stream<T> &&S) {
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
