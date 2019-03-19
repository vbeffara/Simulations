#pragma once
#include <fmt/ostream.h>
#include <gsl/gsl>

namespace vb {
    struct coo {
        int64_t x, y;

        void operator+=(const coo &z) { *this = {x + z.x, y + z.y}; }
        void operator-=(const coo &z) { *this = {x - z.x, y - z.y}; }
    };

    constexpr bool operator==(const coo &z1, const coo &z2) { return (z1.x == z2.x) && (z1.y == z2.y); }
    constexpr bool operator!=(const coo &z1, const coo &z2) { return (z1.x != z2.x) || (z1.y != z2.y); }
    constexpr coo  operator+(const coo &z1, const coo &z2) { return {z1.x + z2.x, z1.y + z2.y}; }
    constexpr coo  operator-(const coo &z1, const coo &z2) { return {z1.x - z2.x, z1.y - z2.y}; }
    constexpr coo  operator-(const coo &z) { return {-z.x, -z.y}; }
    constexpr coo  operator*(const coo &z, int64_t d) { return {z.x * d, z.y * d}; }
    constexpr coo  operator*(int64_t d, const coo &z) { return {z.x * d, z.y * d}; }
    constexpr coo  operator/(const coo &z, int64_t d) { return {z.x / d, z.y / d}; }

    constexpr int64_t norm(const coo &z) { return z.x * z.x + z.y * z.y; }
    constexpr int64_t sup(const coo &z) {
        auto xx = (z.x > 0 ? z.x : -z.x), yy = (z.y > 0 ? z.y : -z.y);
        return (xx > yy ? xx : yy);
    }

    class coo3 {
    public:
        int64_t x, y, z;

        bool operator==(const coo3 &c) const { return (x == c.x) && (y == c.y) && (z == c.z); }
        bool operator!=(const coo3 &c) const { return (x != c.x) || (y != c.y) || (z != c.z); }
        coo3 operator+(const coo3 &c) const { return {x + c.x, y + c.y, z + c.z}; }
    };

    constexpr std::array<coo, 8>  dz_  = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}}};
    constexpr std::array<coo3, 6> dz3_ = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};

    constexpr gsl::span<const coo>  dz{dz_};
    constexpr gsl::span<const coo3> dz3{dz3_};

    struct ucoo {
        size_t x, y;
        constexpr ucoo(size_t x, size_t y) : x(x), y(y) {}
        constexpr ucoo(const coo &z) : x(size_t(z.x)), y(size_t(z.y)) {}
        operator coo() { return {int64_t(x), int64_t(y)}; }
    };

    template <typename T, typename U> constexpr U pmod(T k, U n) { return (k %= T(n)) < T(0) ? U(k + T(n)) : U(k); }

    constexpr ucoo wrap(const coo &z, const ucoo &p) { return {pmod(z.x, p.x), pmod(z.y, p.y)}; }

    struct coo_range {
        coo z, r;
        coo_range(ucoo r) : z({0, 0}), r({int64_t(r.x), int64_t(r.y)}) {}
        const coo_range &begin() const { return *this; }
        const coo_range &end() const { return *this; }
        bool             operator!=(const coo_range &) const { return z.y != r.y; }
        void             operator++() { z = (z.x == r.x - 1) ? coo{0, z.y + 1} : coo{z.x + 1, z.y}; }
        coo              operator*() const { return z; }
    };
} // namespace vb

template <> struct fmt::formatter<vb::coo> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::coo &z, FormatContext &ctx) {
        return format_to(ctx.out(), "({},{})", z.x, z.y);
    }
};
