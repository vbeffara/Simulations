#pragma once
#include <fmt/ostream.h>
#include <gsl/gsl>

namespace vb {
    template <typename T> struct coo_2d {
        T                                        x, y;
        template <typename U> constexpr explicit operator coo_2d<U>() const { return {U(x), U(y)}; };
    };

    template <typename T> constexpr bool operator==(const coo_2d<T> &z1, const coo_2d<T> &z2) { return (z1.x == z2.x) && (z1.y == z2.y); }
    template <typename T> constexpr bool operator!=(const coo_2d<T> &z1, const coo_2d<T> &z2) { return (z1.x != z2.x) || (z1.y != z2.y); }

    template <typename T> constexpr coo_2d<T>  operator+(const coo_2d<T> &z1, const coo_2d<T> &z2) { return {z1.x + z2.x, z1.y + z2.y}; }
    template <typename T> constexpr coo_2d<T>  operator-(const coo_2d<T> &z1, const coo_2d<T> &z2) { return {z1.x - z2.x, z1.y - z2.y}; }
    template <typename T> constexpr coo_2d<T> &operator+=(coo_2d<T> &z, const coo_2d<T> &o) { return z = z + o; }
    template <typename T> constexpr coo_2d<T> &operator-=(coo_2d<T> &z, const coo_2d<T> &o) { return z = z - o; }
    template <typename T> constexpr coo_2d<T>  operator-(coo_2d<T> &z) { return {-z.x, -z.y}; }

    template <typename T, typename U> constexpr coo_2d<T> operator*(U d, const coo_2d<T> &z) { return {z.x * T(d), z.y * T(d)}; }
    template <typename T, typename U> constexpr coo_2d<T> operator*(const coo_2d<T> &z, U d) { return {z.x * T(d), z.y * T(d)}; }
    template <typename T, typename U> constexpr coo_2d<T> operator/(const coo_2d<T> &z, U d) { return {z.x / T(d), z.y / T(d)}; }

    template <typename T> constexpr T             pabs(T x) { return x < 0 ? -x : x; }
    template <typename T> constexpr T             pmax(T x, T y) { return x < y ? y : x; }
    template <typename T, typename U> constexpr U pmod(T k, U n) { return (k %= T(n)) < T(0) ? U(k + T(n)) : U(k); }
    template <typename T> constexpr T             norm(const coo_2d<T> &z) { return z.x * z.x + z.y * z.y; }
    template <typename T> constexpr T             sup(const coo_2d<T> &z) { return pmax(pabs(z.x), pabs(z.y)); }

    using coo  = coo_2d<int64_t>;
    using ucoo = coo_2d<size_t>;

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

    constexpr ucoo wrap(const coo &z, const ucoo &p) { return {pmod(z.x, p.x), pmod(z.y, p.y)}; }

    template <typename T> struct coo_range {
        coo_2d<T> z, r1, r2;
        coo_range(coo_2d<T> r) : z({0, 0}), r1({0, 0}), r2(r) {}
        coo_range(coo_2d<T> r1, coo_2d<T> r2) : z(r1), r1(r1), r2(r2) {}
        const coo_range &begin() const { return *this; }
        const coo_range &end() const { return *this; }
        bool             operator!=(const coo_range<T> &) const { return z.y != r2.y; }
        void             operator++() { z = (z.x == r2.x - 1) ? coo_2d<T>{r1.x, z.y + 1} : coo_2d<T>{z.x + 1, z.y}; }
        coo_2d<T>        operator*() const { return z; }
    };
} // namespace vb

template <typename T> struct fmt::formatter<vb::coo_2d<T>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::coo_2d<T> &z, FormatContext &ctx) {
        return format_to(ctx.out(), "({},{})", z.x, z.y);
    }
};
