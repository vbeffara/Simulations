#pragma once
#include <fmt/ostream.h>
#include <gsl/gsl>
#include <vb/util/Stream.h>

namespace vb {
    template <typename T> struct coo_2d {
        T                                        x, y;
        template <typename U> constexpr explicit operator coo_2d<U>() const { return {U(x), U(y)}; }
    };

    template <typename T> constexpr auto operator==(const coo_2d<T> &z1, const coo_2d<T> &z2) -> bool {
        return (z1.x == z2.x) && (z1.y == z2.y);
    }
    template <typename T> constexpr auto operator!=(const coo_2d<T> &z1, const coo_2d<T> &z2) -> bool {
        return (z1.x != z2.x) || (z1.y != z2.y);
    }

    template <typename T, typename U> constexpr auto operator+(const coo_2d<T> &z1, const coo_2d<U> &z2) -> coo_2d<T> {
        return {z1.x + T(z2.x), z1.y + T(z2.y)};
    }
    template <typename T, typename U> constexpr auto operator-(const coo_2d<T> &z1, const coo_2d<U> &z2) -> coo_2d<T> {
        return {z1.x - T(z2.x), z1.y - T(z2.y)};
    }
    template <typename T, typename U> constexpr auto operator+=(coo_2d<T> &z, const coo_2d<U> &o) -> coo_2d<T> & { return z = z + o; }
    template <typename T, typename U> constexpr auto operator-=(coo_2d<T> &z, const coo_2d<U> &o) -> coo_2d<T> & { return z = z - o; }
    template <typename T> constexpr auto             operator-(coo_2d<T> &z) -> coo_2d<T> { return {-z.x, -z.y}; }

    template <typename T, typename U> constexpr auto operator*(U d, const coo_2d<T> &z) -> coo_2d<T> { return {z.x * T(d), z.y * T(d)}; }
    template <typename T, typename U> constexpr auto operator*(const coo_2d<T> &z, U d) -> coo_2d<T> { return {z.x * T(d), z.y * T(d)}; }
    template <typename T, typename U> constexpr auto operator/(const coo_2d<T> &z, U d) -> coo_2d<T> { return {z.x / T(d), z.y / T(d)}; }

    template <typename T> constexpr auto             pabs(T x) -> T { return x < 0 ? -x : x; }
    template <typename T> constexpr auto             pmax(T x, T y) -> T { return x < y ? y : x; }
    template <typename T, typename U> constexpr auto pmod(T k, U n) -> U { return (k %= T(n)) < T(0) ? U(k + T(n)) : U(k); }
    template <typename T> constexpr auto             norm(const coo_2d<T> &z) -> T { return z.x * z.x + z.y * z.y; }
    template <typename T> constexpr auto             sup(const coo_2d<T> &z) -> T { return pmax(pabs(z.x), pabs(z.y)); }

    using coo  = coo_2d<int64_t>;
    using ucoo = coo_2d<size_t>;

    template <typename T> struct coo_3d {
        T                                        x, y, z;
        template <typename U> constexpr explicit operator coo_3d<U>() const { return {U(x), U(y), U(z)}; }

        auto                       operator==(const coo_3d<T> &c) const -> bool { return (x == c.x) && (y == c.y) && (z == c.z); }
        auto                       operator!=(const coo_3d<T> &c) const -> bool { return (x != c.x) || (y != c.y) || (z != c.z); }
        template <typename U> auto operator+(const coo_3d<U> &c) const -> coo_3d<T> { return {x + T(c.x), y + T(c.y), z + T(c.z)}; }
    };

    using coo3  = coo_3d<int64_t>;
    using ucoo3 = coo_3d<size_t>;

    constexpr std::array<coo, 8>  dz_  = {{{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}}};
    constexpr std::array<coo3, 6> dz3_ = {{{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}}};

    constexpr gsl::span<const coo>  dz{dz_};
    constexpr gsl::span<const coo3> dz3{dz3_};

    // TODO: maybe this should be the only way to do coo -> ucoo conversion?
    constexpr auto wrap(const coo &z, const ucoo &p) -> ucoo { return {pmod(z.x, p.x), pmod(z.y, p.y)}; }

    template <typename T> auto coo_range(coo_2d<T> r1, coo_2d<T> r2) -> Stream<coo_2d<T>> {
        for (T y = r1.y; y < r2.y; ++y)
            for (T x = r1.x; x < r2.x; ++x) co_yield coo_2d<T>{x, y};
    }

    template <typename T> auto coo_range(coo_2d<T> r) -> Stream<coo_2d<T>> { return coo_range({0, 0}, r); }
} // namespace vb

template <typename T> struct fmt::formatter<vb::coo_2d<T>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::coo_2d<T> &z, FormatContext &ctx) const {
        return fmt::format_to(ctx.out(), "({},{})", z.x, z.y);
    }
};
