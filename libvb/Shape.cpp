#include <vb/Shape.h>

namespace vb {
    void Segment::draw(cairo_t *cr) {
        cairo_move_to(cr, z1.real(), z1.imag());
        cairo_line_to(cr, z2.real(), z2.imag());
    }

    void Dot::draw(cairo_t * /*cr*/) {}

    void Circle::draw(cairo_t *cr) {
        cairo_new_sub_path(cr);
        cairo_arc(cr, z.real(), z.imag(), r, 0, 2 * M_PI);
    }

    auto Path::left() -> double {
        double m = z[0].real();
        for (size_t i = 1; i < z.size(); ++i)
            if (std::isnormal(real(z[i]))) m = std::min(m, z[i].real());
        return m;
    }

    auto Path::right() -> double {
        double m = z[0].real();
        for (size_t i = 1; i < z.size(); ++i)
            if (std::isnormal(real(z[i]))) m = std::max(m, z[i].real());
        return m;
    }

    auto Path::top() -> double {
        double m = z[0].imag();
        for (size_t i = 1; i < z.size(); ++i)
            if (std::isnormal(real(z[i]))) m = std::max(m, z[i].imag());
        return m;
    }

    auto Path::bottom() -> double {
        double m = z[0].imag();
        for (size_t i = 1; i < z.size(); ++i)
            if (std::isnormal(real(z[i]))) m = std::min(m, z[i].imag());
        return m;
    }

    void Path::draw(cairo_t *cr) {
        bool down = false;
        int  l    = 0;
        for (auto pp : z) {
            if (std::isnan(real(pp))) {
                down = false;
                continue;
            }
            if (!down) {
                if ((l > 100) && (!this->p.ff)) {
                    cairo_stroke(cr);
                    l = 0;
                }
                cairo_move_to(cr, pp.real(), pp.imag());
                if (!std::isnan(real(pp))) down = true;
            } else {
                cairo_line_to(cr, pp.real(), pp.imag());
            }
            ++l;
        }
    }

    void Polygon::draw(cairo_t *cr) {
        cairo_move_to(cr, z.back().real(), z.back().imag());
        for (auto &zz : z) cairo_line_to(cr, zz.real(), zz.imag());
    }
} // namespace vb
