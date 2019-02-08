#pragma once /// @file
#include <vb/Picture.h>
#include <vb/Shape.h>

namespace vb {
    class Figure : public Picture {
    public:
        Figure(const Hub &H);

        double  left();
        double  right();
        double  top();
        double  bottom();
        Figure &add(std::unique_ptr<Shape> &&S);

        std::vector<std::unique_ptr<Shape>> contents;

        using Picture::output;
        void output(const Hub &H, const std::string &s) override;
        void output_pdf(const Hub &H, const std::string &s = "");

        double margin = 0;
        bool   ortho  = true;

    protected:
        void paint() override;
        void paint(cairo_t *cr, bool fill = true, bool crop = false);

        double basewidth = NAN;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Figure") {
        char * argv[] = {"test_figure"};
        Hub    H("Testing Figure", 1, argv);
        Figure F{H};
        for (int i = 0; i < 10; ++i)
            F.add(std::make_unique<Segment>(cpx(prng.uniform_real(-5, 5), prng.uniform_real(-5, 5)),
                                            cpx(prng.uniform_real(-5, 5), prng.uniform_real(-5, 5)), Pen(Indexed(i, .6, .9))));

        for (int i = 1; i <= 5; ++i) F.add(std::make_unique<Circle>(cpx(0, 0), i));
        F.add(std::make_unique<Dot>(cpx(0, 0)));
        std::vector<cpx> zs1{{0, 1}, {1, 1}, {1, 0}};
        F.add(std::make_unique<Polygon>(zs1));
        std::vector<cpx> zs2{{0.1, 1}, {1.1, 1}, {1.1, 0}};
        F.add(std::make_unique<Path>(zs2));
        F.show();
        F.hide();
    }
#endif
} // namespace vb
