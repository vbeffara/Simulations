#pragma once
#include <vb/Picture.h>
#include <vb/util/cpx.h>

namespace vb {
    class Coloring : public Picture {
    public:
        Coloring(const std::string &s, cpx z1_, cpx z2_, size_t n, std::function<Color(cpx)> f_);

        void show() override;
        void show2();

        void scale(double s);
        void shift(cpx z);

        double                    eps;
        bool                      aa = true;
        cpx                       z1, z2;
        std::function<Color(cpx)> f;

    protected:
        Color &at(coo z) const;
        cpx    c_to_z(coo c) const;
        double detail = 0;

    private:
        Color aa_color(coo c, bool pre = false) const;

        void line(coo s, coo d, int64_t l);
        void tessel_go(coo ul, coo lr);
        void tessel(coo ul, coo lr);
        void do_aa();

        gsl::span<Color> stage;

        int handle(int event) override;
        int pixel_detail = 0;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Coloring") {
        Coloring C("Testing Coloring", cpx(0, 0), cpx(1, 1), 500, [](cpx z) { return Indexed(int(real(z) * imag(z) * 10)); });
        C.scale(1.2);
        C.shift(cpx(.2, .3));
        C.show();
        C.hide();
    }
#endif
} // namespace vb
