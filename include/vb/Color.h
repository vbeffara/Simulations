#pragma once
#include <cstdint>

namespace vb {
    struct Color {
        constexpr Color(uint8_t R, uint8_t G, uint8_t B, uint8_t A = 255) noexcept : b(B), g(G), r(R), a(A) {}
        constexpr Color() : Color(0, 0, 0, 0) {}

        auto operator==(const Color &o) const -> bool { return (r == o.r) && (g == o.g) && (b == o.b) && (a == o.a); }
        auto operator!=(const Color &o) const -> bool { return (r != o.r) || (g != o.g) || (b != o.b) || (a != o.a); }

        explicit operator int() const { return (r + g + b) / 3; }
        explicit operator uint8_t() const { return uint8_t((int(r) + int(g) + int(b)) / 3); }

        uint8_t b, g, r, a;
    };

    template <typename T> auto to_Color(T t) -> Color { return static_cast<Color>(t); }

    constexpr Color NOCOLOR{0, 0, 0, 0}, BLACK{0, 0, 0}, WHITE{255, 255, 255}, RED{255, 0, 0}, GREEN{0, 255, 0}, BLUE{0, 0, 255},
        CYAN{0, 255, 255}, MAGENTA{255, 0, 255}, YELLOW{255, 255, 0};

    constexpr auto Grey(uint8_t x) noexcept -> Color { return {x, x, x}; }

    auto HSV(double h, double s, double v) noexcept -> Color;
    auto Indexed(int i, double s = 1, double v = 1) noexcept -> Color;
} // namespace vb
