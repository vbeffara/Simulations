#pragma once /// @file
#include <fmt/ostream.h>
#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    using Cycles = std::vector<std::vector<unsigned>>;

    class Passport : public std::vector<std::pair<int, int>> {};

    class Permutation : public std::vector<unsigned> {
    public:
        Permutation(unsigned n = 0) : std::vector<unsigned>(n) {
            for (unsigned i = 0; i < n; ++i) at(i) = i;
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

    Stream<Permutation> permutations(int n);
    Stream<Permutation> permutations(std::vector<int> s);

#ifdef UNIT_TESTS
    TEST_CASE("vb::Permutation") {
        Permutation P1 = Transposition(4, 0, 1), P2 = Transposition(4, 0, 2), P = P1 * P2;
        Passport    PP = P.passport();
        CHECK(PP[0].first == 3);
    }
#endif
} // namespace vb

template <> struct fmt::formatter<vb::Permutation> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Permutation &P, FormatContext &ctx) {
        std::vector<std::string> tmp;
        for (const auto &cc : P.cycles()) tmp.push_back(fmt::format("({})", fmt::join(cc, " ")));
        return format_to(ctx.out(), "({})", fmt::join(tmp, " "));
    }
};

template <> struct fmt::formatter<vb::Passport> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Passport &P, FormatContext &ctx) {
        std::vector<std::string> tmp;
        for (const auto &c : P) tmp.push_back(fmt::format("{}({})", c.first, c.second));
        return format_to(ctx.out(), "{}", fmt::join(tmp, " "));
    }
};
