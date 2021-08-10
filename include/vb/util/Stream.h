#pragma once
#include <range/v3/experimental/utility/generator.hpp>

namespace vb {
    template <typename T> using Stream = ranges::experimental::generator<T>;

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

    template <typename T> auto take(size_t n, Stream<T> &&S) -> Stream<T> {
        if (n > 0) {
            for (const auto &x : S) {
                co_yield(x);
                --n;
                if (n == 0) break;
            }
        }
    }

    template <typename F, typename T> auto filter(F f, Stream<T> &&S) -> Stream<T> {
        for (const auto &x : S)
            if (f(x)) co_yield(x);
    }

    template <typename F, typename T> auto fmap(F f, Stream<T> &&S) -> Stream<decltype(f(T()))> {
        for (const auto &x : S) co_yield(f(x));
    }
} // namespace vb
