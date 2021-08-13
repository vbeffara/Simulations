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
        [[nodiscard]] auto at(ucoo z) const -> Color &;
        [[nodiscard]] auto c_to_z(ucoo c) const -> cpx;
        double             detail = 0;

    private:
        [[nodiscard]] auto aa_color(ucoo c, bool pre = false) const -> Color;

        void line(ucoo s, coo d, size_t l);
        void tessel_go(ucoo ul, ucoo lr);
        void tessel(ucoo ul, ucoo lr);
        void do_aa();

        gsl::span<Color> stage;

        auto   handle(int event) -> int override;
        size_t pixel_detail = 0;
    };
} // namespace vb
