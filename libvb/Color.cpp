#include <vb/Color.h>
#include <fmt/format.h>

namespace vb {
    Color Grey(uint8_t x) noexcept { return {x, x, x}; }

    Color HSV(double h, double s, double v) noexcept {
        auto   h_i = int(h * 6);
        double f   = h * 6 - h_i;
        v *= 255;
        auto p = uint8_t(v * (1 - s)), q = uint8_t(v * (1 - f * s)), t = uint8_t(v * (1 - (1 - f) * s));
        if (h_i == 0) return {uint8_t(v), t, p};
        if (h_i == 1) return {q, uint8_t(v), p};
        if (h_i == 2) return {p, uint8_t(v), t};
        if (h_i == 3) return {p, q, uint8_t(v)};
        if (h_i == 4) return {t, p, uint8_t(v)};
        return {uint8_t(v), p, q};
    }

    Color Indexed(int i, double s, double v) noexcept {
        double x = i * 1.61803398874989484820;
        return HSV(x - int(x), s, v);
    }

    std::ostream &operator<<(std::ostream &o, const Color &c) { return o << fmt::format("RGBA({},{},{},{})", c.r, c.g, c.b, c.a); }
} // namespace vb
