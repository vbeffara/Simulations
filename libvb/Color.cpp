#include <vb/Color.h>
#include <spdlog/fmt/fmt.h>

namespace vb {
    Color Grey(uint8_t x) noexcept { return Color(x, x, x); }

    Color HSV(double h, double s, double v) noexcept {
        int    h_i = h * 6;
        double f   = h * 6 - h_i;
        v *= 255;
        int p = v * (1 - s), q = v * (1 - f * s), t = v * (1 - (1 - f) * s);
        if (h_i == 0) return Color(v, t, p);
        if (h_i == 1) return Color(q, v, p);
        if (h_i == 2) return Color(p, v, t);
        if (h_i == 3) return Color(p, q, v);
        if (h_i == 4) return Color(t, p, v);
        return Color(v, p, q);
    }

    Color Indexed(int i, double s, double v) noexcept {
        double x = i * 1.61803398874989484820;
        return HSV(x - int(x), s, v);
    }

    std::ostream & operator<<(std::ostream & o, const Color & c) { return o << fmt::format("RGBA({},{},{},{})", c.r, c.g, c.b, c.a); }
} // namespace vb
