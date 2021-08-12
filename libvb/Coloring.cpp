#include <tbb/parallel_for.h>
#include <tbb/task_group.h>
#include <vb/Coloring.h>
#include <vb/util/misc.h>

namespace vb {
    Coloring::Coloring(const std::string &s, cpx z1_, cpx z2_, size_t n, std::function<Color(cpx)> f_)
        : Picture(s, {n, size_t(n * imag(z2_ - z1_) / real(z2_ - z1_))}), eps(real(z1_ - z2_) / double(n)), z1(z1_), z2(z2_),
          f(std::move(f_)) {}

    void Coloring::show() {
        Picture::show();
        auto sd      = static_cast<Color *>(static_cast<void *>(cairo_image_surface_get_data(surface)));
        stage        = gsl::span<Color>(sd, stride * to_unsigned(pixel_h()));
        eps          = real(z2 - z1) / pixel_w();
        pixel_detail = size_t(detail / eps);
        for (size_t i = 0; i < to_unsigned(pixel_w()); ++i)
            for (size_t j = 0; j < to_unsigned(pixel_h()); ++j) at({i, j}) = BLACK;
        run([&] { tessel({0, 0}, {to_unsigned(pixel_w()) - 1, to_unsigned(pixel_h()) - 1}); });
        if (aa) run([&]() { do_aa(); });
        update();
    }

    void Coloring::do_aa() {
        auto pw = to_unsigned(pixel_w()), ph = to_unsigned(pixel_h());
        tbb::parallel_for(0, to_signed(pw * ph), [=](int i) {
            auto [x, y] = std::div(i, to_signed(ph));
            ucoo  c{to_unsigned(x), to_unsigned(y)};
            Color cc = at(c);
            bool  u  = true;
            for (size_t d = 0; d < 4; ++d) {
                auto c2 = c + dz[d];
                if ((c2.x >= 0) && (c2.x < pw) && (c2.y >= 0) && (c2.y < ph) && (at(c2) != cc)) u = false;
            }
            if (!(u || die)) at(c) = aa_color(c, true);
        });
    }

    void Coloring::scale(double s) {
        cpx mid = (z1 + z2) / 2.0;
        z1      = mid + s * (z1 - mid);
        z2      = mid + s * (z2 - mid);
    }
    void Coloring::shift(cpx z) {
        z1 += z;
        z2 += z;
    }

    auto Coloring::c_to_z(ucoo c) const -> cpx { return z1 + cpx(double(c.x), double(c.y)) * eps; }

    auto Coloring::at(ucoo z) const -> Color & { return stage[z.x + stride * z.y]; }

    auto Coloring::aa_color(ucoo c, bool pre) const -> Color {
        cpx z = c_to_z(c);
        int r(0), g(0), b(0), a(0);
        if (pre) {
            Color C = at(c);
            r       = C.r;
            g       = C.g;
            b       = C.b;
            a       = C.a;
        }
        for (int i = -1; i <= 1; ++i)
            for (int j = -1; j <= 1; ++j)
                if ((!pre) || (i != 0) || (j != 0)) {
                    Color cc = f(z + eps * cpx(i, j) / 3.0);
                    r += cc.r;
                    g += cc.g;
                    b += cc.b;
                    a += cc.a;
                }
        return Color(uint8_t(r / 9), uint8_t(g / 9), uint8_t(b / 9), uint8_t(a / 9));
    }

    void Coloring::line(ucoo s, coo d, size_t l) {
        tbb::parallel_for(size_t(0), l, [=](size_t i) {
            ucoo c = s + d * i;
            if (!die) at(c) = f(c_to_z(c));
        });
    }

    void Coloring::tessel_go(ucoo ul, ucoo lr) {
        auto size = std::min(lr.x - ul.x, lr.y - ul.y);
        if (size <= 1) return;

        auto  z    = ul;
        Color tmp  = at(ul);
        bool  mono = true;
        if ((pixel_detail != 0) && (size > pixel_detail)) mono = false;
        for (; mono && (z != ucoo{lr.x, ul.y}); z += coo{1, 0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != ucoo{lr.x, lr.y}); z += coo{0, 1}) mono = mono && (at(z) == tmp);
        for (; mono && (z != ucoo{ul.x, lr.y}); z += coo{-1, 0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != ucoo{ul.x, ul.y}); z += coo{0, -1}) mono = mono && (at(z) == tmp);

        if (mono) {
            for (auto i = ul.x + 1; i < lr.x; ++i)
                for (auto j = ul.y + 1; j < lr.y; ++j) at({i, j}) = tmp;
            return;
        }

        auto ul_ = (lr.x - ul.x > lr.y - ul.y) ? ucoo{(ul.x + lr.x) / 2, ul.y} : ucoo{ul.x, (ul.y + lr.y) / 2};
        auto lr_ = (lr.x - ul.x > lr.y - ul.y) ? ucoo{(ul.x + lr.x) / 2, lr.y} : ucoo{lr.x, (ul.y + lr.y) / 2};
        auto dd_ = (lr.x - ul.x > lr.y - ul.y) ? coo{0, 1} : coo{1, 0};

        line(ul_, dd_, size);
        tbb::task_group g;
        g.run([=]() { tessel_go(ul, lr_); });
        g.run([=]() { tessel_go(ul_, lr); });
        g.wait();
    }

    void Coloring::tessel(ucoo ul, ucoo lr) {
        tbb::task_group g;
        g.run([=]() { line(ul, {1, 0}, lr.x - ul.x); });
        g.run([=]() { line({lr.x, ul.y}, {0, 1}, lr.y - ul.y); });
        g.run([=]() { line(lr, {-1, 0}, lr.x - ul.x); });
        g.run([=]() { line({ul.x, lr.y}, {0, -1}, lr.y - ul.y); });
        g.wait();
        tessel_go(ul, lr);
    }

    auto Coloring::handle(int event) -> int {
        if (event == FL_KEYDOWN) switch (Fl::event_key()) {
            case '-':
                scale(1.25);
                show();
                return 1;
                break;
            case '+':
            case '=':
                scale(0.80);
                show();
                return 1;
                break;
            case 'a':
                aa = !aa;
                show();
                return 1;
                break;
            case FL_Left:
                shift(cpx(+.1, 0) * real(z2 - z1));
                show();
                return 1;
                break;
            case FL_Right:
                shift(cpx(-.1, 0) * real(z2 - z1));
                show();
                return 1;
                break;
            case FL_Up:
                shift(cpx(0, -.1) * real(z2 - z1));
                show();
                return 1;
                break;
            case FL_Down:
                shift(cpx(0, +.1) * real(z2 - z1));
                show();
                return 1;
                break;
            }
        return Picture::handle(event);
    }
} // namespace vb
