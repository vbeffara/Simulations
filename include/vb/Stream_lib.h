#pragma once
#include <vb/Hypermap.h>
#include <vb/Stream.h>
#include <vb/data/Array.h>

namespace vb {
    Stream<std::vector<int>> partitions(int n, int m = 1); // Partitions of n with piece size at least m
    Stream<std::vector<int>> tuples(int k, int n);         // Ordered, distinct k-tuples in [0,n-1]
    Stream<std::vector<int>> cycles(int k, int n);         // k-tuples up to cyclic permutation

    Stream<Permutation> permutations(int n);
    Stream<Permutation> permutations(std::vector<int> s);

    Stream<Hypermap> hypermaps(const std::vector<int> &s, const std::vector<int> &a, const std::vector<int> &p);

#ifdef UNIT_TESTS
    TEST_CASE("vb::Stream lib") {
        CHECK(size(cycles(3, 6)) == 40);
        CHECK(size(permutations(7)) == fact(7));
        CHECK(size(hypermaps({2, 2, 2}, {2, 2, 2}, {3, 3})) == 1);

        int n = 0;
        for (const auto &c : partitions(6)) n += size(permutations(c));
        CHECK(n == fact(6));
    }
#endif
} // namespace vb
