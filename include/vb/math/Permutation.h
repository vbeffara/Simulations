#pragma once /// @file
#include <fmt/ostream.h>
#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    using Cycle  = std::vector<size_t>;
    using Cycles = std::vector<Cycle>;

    class Passport : public std::vector<std::pair<int, int>> {};

    class Permutation : public std::vector<size_t> {
    public:
        Permutation(size_t n = 0) : std::vector<size_t>(n) {
            for (size_t i = 0; i < n; ++i) at(i) = i;
        }
        Permutation(std::vector<size_t> s) : std::vector<size_t>(std::move(s)) {}
        Permutation(Cycles &c);

        bool is_identity() const;

        Permutation inverse() const;
        Permutation operator*(const Permutation &o) const; // this then o i.e. $this * o = o \circ this$.
        Permutation conjugate(const Permutation &s) const;

        Cycles              cycles() const;
        std::vector<size_t> signature() const;
        Passport            passport() const;
    };

    Permutation Transposition(size_t n, size_t i, size_t j);

    bool connected(const Permutation &s, const Permutation &a);

    Stream<Permutation> permutations(size_t n);
    Stream<Permutation> permutations(const std::vector<size_t> &s);
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
