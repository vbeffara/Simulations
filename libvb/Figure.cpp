#include <cairo/cairo-pdf.h>
#include <vb/Figure.h>

namespace vb {
    Figure::Figure(const std::string &s) : Picture(s, {600, 600}) {}

    auto Figure::left() -> double {
        double l = 0.0;
        for (const auto &c : contents) l = std::min(l, c->left());
        return l - margin;
    }

    auto Figure::right() -> double {
        double l = 0.0;
        for (const auto &c : contents) l = std::max(l, c->right());
        return l + margin;
    }

    auto Figure::top() -> double {
        double l = 0.0;
        for (const auto &c : contents) l = std::max(l, c->top());
        return l + margin;
    }

    auto Figure::bottom() -> double {
        double l = 0.0;
        for (const auto &c : contents) l = std::min(l, c->bottom());
        return l - margin;
    }

    auto Figure::add(std::unique_ptr<Shape> &&S) -> Figure & {
        contents.push_back(std::move(S));
        return (*this);
    }

    void Figure::paint(cairo_t *cr, bool fill, bool crop) {
        double const real_h = w() * (top() - bottom()) / (right() - left());
        double wd = right() - left(), mid_x = (right() + left()) / 2;
        double ht = top() - bottom(), mid_y = (top() + bottom()) / 2;

        double scale_x = w() / wd, scale_y = (crop ? real_h : h()) / ht;
        double const scale = std::min(scale_x, scale_y);
        if (ortho) scale_x = scale_y = scale;
        basewidth = 1.0 / scale;

        if (fill) {
            cairo_save(cr);
            cairo_set_source_rgb(cr, 1, 1, 1);
            cairo_paint(cr);
            cairo_restore(cr);
        }

        cairo_save(cr);
        cairo_translate(cr, w() * .5, (crop ? real_h : h()) * .5);
        cairo_scale(cr, scale_x * .98, -scale_y * .98); // A tiny margin for stoke width.
        cairo_translate(cr, -mid_x, -mid_y);
        cairo_set_line_width(cr, basewidth);
        cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
        cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);

        for (const auto &c : contents) {
            cairo_save(cr);
            cairo_set_source_rgb(cr, c->p.c.r / 255.0, c->p.c.g / 255.0, c->p.c.b / 255.0);
            cairo_set_line_width(cr, basewidth * c->p.w);
            c->draw(cr);
            if (c->p.ff) {
                cairo_stroke_preserve(cr);
                cairo_set_source_rgba(cr, c->p.f.r / 255.0, c->p.f.g / 255.0, c->p.f.b / 255.0, c->p.f.a / 255.0);
                cairo_fill(cr);
            }
            cairo_stroke(cr);
            cairo_restore(cr);
        }

        cairo_restore(cr);
    }

    void Figure::paint() { paint(context); }

    void Figure::output_pdf(const std::string &s) {
        std::string const os = s + ".pdf";

        double const real_h = w() * (top() - bottom()) / (right() - left());

        cairo_surface_t *pdf = cairo_pdf_surface_create(os.c_str(), w(), real_h);
        cairo_t *        pcr = cairo_create(pdf);
        paint(pcr, false, true);
        cairo_show_page(pcr);
        cairo_destroy(pcr);
        cairo_surface_destroy(pdf);
    }

    void Figure::output(const std::string &s) { output_pdf(s); }
} // namespace vb
