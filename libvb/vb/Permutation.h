#pragma once /// @file
#include <iostream>
#include <vector>

namespace vb {
    using Cycles = std::vector<std::vector<unsigned>>;

    class Passport : public std::vector<std::pair<int, int>> {};

    class Permutation : public std::vector<unsigned> {
    public:
        Permutation(int n = 0) : std::vector<unsigned>(n) {
            for (int i = 0; i < n; ++i) at(i) = i;
        }
        Permutation(std::vector<unsigned> s) : std::vector<unsigned>(std::move(s)) {}
        Permutation(Cycles &c);

        bool is_identity() const;

        Permutation inverse() const;
        Permutation operator*(const Permutation &o) const; // this then o i.e. $this * o = o \circ this$.
        Permutation conjugate(const Permutation &s) const;

        Cycles           cycles() const;
        std::vector<int> signature() const;
        Passport         passport() const;
    };

    Permutation Transposition(int n, int i, int j);

    bool connected(const Permutation &s, const Permutation &a);

    std::ostream &operator<<(std::ostream &os, const Passport &P);
    std::ostream &operator<<(std::ostream &os, const Permutation &P);

#ifdef UNIT_TESTS
    TEST_CASE("vb::Permutation") {
        Permutation P1 = Transposition(4, 0, 1), P2 = Transposition(4, 0, 2), P = P1 * P2;
        Passport    PP = P.passport();
        CHECK(PP[0].first == 3);
    }
#endif
} // namespace vb
