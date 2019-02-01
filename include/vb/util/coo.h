#pragma once /// @file
#include <complex>
#include <gsl/gsl>

namespace vb {
    struct coo {
        int64_t x, y;

        void operator+=(const coo &z) { *this = {x + z.x, y + z.y}; }
        void operator-=(const coo &z) { *this = {x - z.x, y - z.y}; }

        template <typename T> explicit operator std::complex<T>() const { return {T(x), T(y)}; }
    };

    inline bool operator==(const coo &z1, const coo &z2) { return (z1.x == z2.x) && (z1.y == z2.y); }
    inline bool operator!=(const coo &z1, const coo &z2) { return (z1.x != z2.x) || (z1.y != z2.y); }
    inline coo  operator+(const coo &z1, const coo &z2) { return {z1.x + z2.x, z1.y + z2.y}; }
    inline coo  operator-(const coo &z1, const coo &z2) { return {z1.x - z2.x, z1.y - z2.y}; }
    inline coo  operator-(const coo &z) { return {-z.x, -z.y}; }
    inline coo  operator*(const coo &z, int64_t d) { return {z.x * d, z.y * d}; }
    inline coo  operator/(const coo &z, int64_t d) { return {z.x / d, z.y / d}; }

    inline int64_t norm(coo z) { return z.x * z.x + z.y * z.y; }
    inline int64_t sup(coo z) { return std::max(std::abs(z.x), std::abs(z.y)); }

    inline std::ostream &operator<<(std::ostream &os, const coo z) { return os << "(" << z.x << "," << z.y << ")"; }

    class coo3 {
    public:
        bool operator==(const coo3 &c) const { return (x == c.x) && (y == c.y) && (z == c.z); }
        bool operator!=(const coo3 &c) const { return (x != c.x) || (y != c.y) || (z != c.z); }
        coo3 operator+(const coo3 &c) const { return {x + c.x, y + c.y, z + c.z}; }

        int64_t x, y, z;
    };

    inline const coo  dz_[]  = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {-1, -1}, {-1, 1}, {1, -1}};
    inline const coo3 dz3_[] = {{1, 0, 0}, {-1, 0, 0}, {0, 1, 0}, {0, -1, 0}, {0, 0, 1}, {0, 0, -1}};

    inline const gsl::span<const coo>  dz{dz_};
    inline const gsl::span<const coo3> dz3{dz3_};

#ifdef UNIT_TESTS
    TEST_CASE("vb::coo") {
        coo z1{2, 3}, z2{4, -1};
        CHECK(z1 + z2 == coo{6, 2});
        CHECK(z1 - z2 == coo{-2, 4});
        CHECK(-z1 == coo{-2, -3});
        CHECK(z1 * 3 == coo{6, 9});
        CHECK(z1 / 2 == coo{1, 1});

        z1 += z2;
        z2 -= z1;
        CHECK(std::complex<double>(z2) == std::complex<double>(-2, -3));
        CHECK(norm(z2) == 13);
        CHECK(sup(z2) == 3);
    }
#endif
} // namespace vb
