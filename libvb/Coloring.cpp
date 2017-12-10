#include <vb/Coloring.h>

namespace vb {
    Coloring::Coloring(cpx z1_, cpx z2_, int n, std::function<Color(cpx)> f_)
        : Picture(n, n * imag(z2_ - z1_) / real(z2_ - z1_)), eps(real(z1_ - z2_) / n), z1(z1_), z2(z2_), f(std::move(f_)) {}

    void Coloring::show() {
        Picture::show();
        auto sd      = static_cast<Color *>(static_cast<void *>(cairo_image_surface_get_data(surface)));
        stage        = gsl::span<Color>(sd, stride * pixel_h());
        eps          = real(z2 - z1) / pixel_w();
        pixel_detail = detail / eps;
        for (int i = 0; i < pixel_w(); ++i)
            for (int j = 0; j < pixel_h(); ++j) at(coo(i, j)) = BLACK;
        run([&] { tessel({0, 0}, {pixel_w() - 1, pixel_h() - 1}); });
        if (aa) run([&]() { do_aa(); });
        update();
    }

    void Coloring::do_aa() {
        int              pw = pixel_w(), ph = pixel_h();
        std::vector<coo> cs;
        for (int y = 0; y < ph; ++y) {
            for (int x = 0; x < pw; ++x) {
                coo   c{x, y};
                Color cc = at(c);
                bool  u  = true;
                for (int d = 0; d < 4; ++d) {
                    coo c2 = c + dz[d];
                    if ((c2.x >= 0) && (c2.x < pw) && (c2.y >= 0) && (c2.y < ph) && (at(c2) != cc)) u = false;
                }
                if (!u) cs.push_back(c);
            }
        }
#pragma omp parallel for schedule(dynamic)
        for (int i = 0; i < cs.size(); ++i)
            if (!die) at(cs[i]) = aa_color(cs[i], true); // NOLINT
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

    cpx Coloring::c_to_z(coo c) const { return z1 + cpx(c) * eps; }

    Color & Coloring::at(coo z) const { return stage[z.x + stride * z.y]; }

    Color Coloring::aa_color(coo c, bool pre) const {
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
                    Color c = f(z + eps * cpx(i, j) / 3.0);
                    r += c.r;
                    g += c.g;
                    b += c.b;
                    a += c.a;
                }
        return Color(r / 9, g / 9, b / 9, a / 9);
    }

    Project Coloring::line(coo s, coo d, int l) {
        if (l > 20) {
            int l2 = l / 2;
            return {[=] { return line(s, d, l2); }, [=] { return line(s + d * l2, d, l - l2); }};
        }
        for (int i = 0; i < l; ++i) {
            coo c = s + d * i;
            if (!die) at(c) = f(c_to_z(c));
        }
        return {};
    }

    Project Coloring::tessel_go(coo ul, coo lr) {
        int size = std::min(lr.x - ul.x, lr.y - ul.y);
        if (size <= 1) return {};

        coo   z    = ul;
        Color tmp  = at(ul);
        bool  mono = true;
        if ((pixel_detail != 0) && (size > pixel_detail)) mono = false;
        for (; mono && (z != coo{lr.x, ul.y}); z += {1, 0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo{lr.x, lr.y}); z += {0, 1}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo{ul.x, lr.y}); z += {-1, 0}) mono = mono && (at(z) == tmp);
        for (; mono && (z != coo{ul.x, ul.y}); z += {0, -1}) mono = mono && (at(z) == tmp);

        if (mono) {
            for (int i = ul.x + 1; i < lr.x; ++i)
                for (int j = ul.y + 1; j < lr.y; ++j) at(coo(i, j)) = tmp;
            return {};
        }

        coo ul_ = (lr.x - ul.x > lr.y - ul.y) ? coo{(ul.x + lr.x) / 2, ul.y} : coo{ul.x, (ul.y + lr.y) / 2};
        coo lr_ = (lr.x - ul.x > lr.y - ul.y) ? coo{(ul.x + lr.x) / 2, lr.y} : coo{lr.x, (ul.y + lr.y) / 2};
        coo dd_ = (lr.x - ul.x > lr.y - ul.y) ? coo{0, 1} : coo{1, 0};

        Project p{[=] { return line(ul_, dd_, size); }};
        return p.then([=] { return Project{[=] { return tessel_go(ul, lr_); }, [=] { return tessel_go(ul_, lr); }}; });
    }

    void Coloring::tessel(coo ul, coo lr) {
        Project p{[=] { return line(ul, coo(1, 0), lr.x - ul.x); }, [=] { return line(coo(lr.x, ul.y), coo(0, 1), lr.y - ul.y); },
                  [=] { return line(lr, coo(-1, 0), lr.x - ul.x); }, [=] { return line(coo(ul.x, lr.y), coo(0, -1), lr.y - ul.y); }};
        p.then([=] { return tessel_go(ul, lr); });
        execute_par(p);
    }

    int Coloring::handle(int event) {
        if (event == FL_KEYDOWN) switch (Fl::event_key()) {
            case '-':
                scale(1.25);
                show();
                return 1;
                break;
            case '+':
                scale(0.80);
                show();
                return 1;
                break;
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
