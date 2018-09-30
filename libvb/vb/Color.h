#pragma once /// @file
#include <cstdint>
#include <iostream>

namespace vb {
    struct Color {
#ifdef BIGENDIAN
        uint8_t a, r, g, b;
        Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) noexcept : a(A), r(R), g(G), b(B) {}
#else
        uint8_t b, g, r, a;
        Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) noexcept : b(B), g(G), r(R), a(A) {}
#endif
        Color() : Color(0, 0, 0, 0) {}

        bool operator==(const Color & o) const { return (r == o.r) && (g == o.g) && (b == o.b) && (a == o.a); }
        bool operator!=(const Color & o) const { return (r != o.r) || (g != o.g) || (b != o.b) || (a != o.a); }

        explicit operator int() { return (r + g + b) / 3; }
    };

    template <typename T> Color to_Color(T t) { return static_cast<Color>(t); }

    const Color NOCOLOR(0, 0, 0, 0), BLACK(0, 0, 0), WHITE(255, 255, 255), RED(255, 0, 0), GREEN(0, 255, 0), BLUE(0, 0, 255),
        CYAN(0, 255, 255), MAGENTA(255, 0, 255), YELLOW(255, 255, 0);

    Color Grey(uint8_t x) noexcept;
    Color HSV(double h, double s, double v) noexcept;
    Color Indexed(int i, double s = 1, double v = 1) noexcept;

    std::ostream & operator<<(std::ostream & o, const Color & c);

#ifdef UNIT_TESTS
    TEST_CASE("vb::Color") {
        Color c = Grey(123);
        CHECK(c == Color(123, 123, 123));
        CHECK(c != Color(122, 123, 123));
    }
#endif
} // namespace vb
