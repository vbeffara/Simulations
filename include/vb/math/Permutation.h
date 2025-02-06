#pragma once /// @file
#include <fmt/ostream.h>
#include <fmt/ranges.h>
#include <vb/util/Stream.h>
#include <vector>

namespace vb {
    using Cycle  = std::vector<size_t>;
    using Cycles = std::vector<Cycle>;

    class Passport : public std::vector<std::pair<int, int>> {};

    class Permutation : public std::vector<size_t> {
    public:
        explicit Permutation(size_t n = 0) : std::vector<size_t>(n) {
            for (size_t i = 0; i < n; ++i) at(i) = i;
        }
        explicit Permutation(std::vector<size_t> s) : std::vector<size_t>(std::move(s)) {}
        explicit Permutation(Cycles &c);

        [[nodiscard]] auto is_identity() const -> bool;

        [[nodiscard]] auto inverse() const -> Permutation;
        auto               operator*(const Permutation &o) const -> Permutation; // this then o i.e. $this * o = o \circ this$.
        [[nodiscard]] auto conjugate(const Permutation &s) const -> Permutation;

        [[nodiscard]] auto cycles() const -> Cycles;
        [[nodiscard]] auto signature() const -> std::vector<size_t>;
        [[nodiscard]] auto passport() const -> Passport;
    };

    auto Transposition(size_t n, size_t i, size_t j) -> Permutation;

    auto connected(const Permutation &s, const Permutation &a) -> bool;

    auto permutations(size_t n) -> Stream<Permutation>;
    auto permutations(const std::vector<size_t> &s) -> Stream<Permutation>;
} // namespace vb

template <> struct fmt::formatter<vb::Permutation> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Permutation &P, FormatContext &ctx) const {
        std::vector<std::string> tmp;
        for (const auto &cc : P.cycles()) tmp.push_back(fmt::format("({})", fmt::join(cc, " ")));
        return fmt::format_to(ctx.out(), "({})", fmt::join(tmp, " "));
    }
};

template <> struct fmt::formatter<vb::Passport> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Passport &P, FormatContext &ctx) const {
        std::vector<std::string> tmp;
        for (const auto &c : P) tmp.push_back(fmt::format("{}({})", c.first, c.second));
        return fmt::format_to(ctx.out(), "{}", fmt::join(tmp, " "));
    }
};
