#pragma once /// @file
#include <cstdint>

namespace vb {
    struct Color {
        constexpr Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) noexcept : b(B), g(G), r(R), a(A) {}
        constexpr Color() : Color(0, 0, 0, 0) {}

        bool operator==(const Color &o) const { return (r == o.r) && (g == o.g) && (b == o.b) && (a == o.a); }
        bool operator!=(const Color &o) const { return (r != o.r) || (g != o.g) || (b != o.b) || (a != o.a); }

        explicit operator int() { return (r + g + b) / 3; }
        explicit operator uint8_t() { return uint8_t((int(r) + int(g) + int(b)) / 3); }

        uint8_t b, g, r, a;
    };

    template <typename T> Color to_Color(T t) { return static_cast<Color>(t); }

    const Color NOCOLOR{0, 0, 0, 0}, BLACK{0, 0, 0}, WHITE{255, 255, 255}, RED{255, 0, 0}, GREEN{0, 255, 0}, BLUE{0, 0, 255},
        CYAN{0, 255, 255}, MAGENTA{255, 0, 255}, YELLOW{255, 255, 0};

    Color Grey(uint8_t x) noexcept;
    Color HSV(double h, double s, double v) noexcept;
    Color Indexed(int i, double s = 1, double v = 1) noexcept;
} // namespace vb
