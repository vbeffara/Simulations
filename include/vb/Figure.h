#pragma once /// @file
#include <vb/Picture.h>
#include <vb/Shape.h>

namespace vb {
    class Figure : public Picture {
    public:
        Figure(const std::string &s);

        double  left();
        double  right();
        double  top();
        double  bottom();
        Figure &add(std::unique_ptr<Shape> &&S);

        std::vector<std::unique_ptr<Shape>> contents;

        using Picture::output;
        void output(const std::string &s) override;
        void output_pdf(const std::string &s);

        double margin = 0;
        bool   ortho  = true;

    protected:
        void paint() override;
        void paint(cairo_t *cr, bool fill = true, bool crop = false);

        double basewidth = NAN;
    };
} // namespace vb
