#pragma once
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>

namespace vb {
    template <typename T> using Stream = ranges::experimental::generator<T>;

    auto partitions(size_t n, size_t m = 1) -> Stream<std::vector<size_t>>; // Partitions of n with piece size at least m
    auto tuples(size_t k, size_t n) -> Stream<std::vector<size_t>>;         // Ordered, distinct k-tuples in [0,n-1]
    auto cycles(size_t k, size_t n) -> Stream<std::vector<size_t>>;         // k-tuples up to cyclic permutation
} // namespace vb
