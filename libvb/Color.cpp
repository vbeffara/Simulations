#include <vb/Color.h>

namespace vb {
    auto HSV(double h, double s, double v) noexcept -> Color {
        auto   h_i = int(h * 6);
        double const f   = h * 6 - h_i;
        v *= 255;
        auto p = uint8_t(v * (1 - s)), q = uint8_t(v * (1 - f * s)), t = uint8_t(v * (1 - (1 - f) * s));
        if (h_i == 0) return {uint8_t(v), t, p};
        if (h_i == 1) return {q, uint8_t(v), p};
        if (h_i == 2) return {p, uint8_t(v), t};
        if (h_i == 3) return {p, q, uint8_t(v)};
        if (h_i == 4) return {t, p, uint8_t(v)};
        return {uint8_t(v), p, q};
    }

    auto Indexed(int i, double s, double v) noexcept -> Color {
        double const x = i * 1.61803398874989484820;
        return HSV(x - int(x), s, v);
    }
} // namespace vb
