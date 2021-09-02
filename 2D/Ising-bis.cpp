#include <vb/Bitmap.h>
#include <vb/Figure.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> auto to_Color(int t) -> Color { return t != 0 ? Indexed((3 + t) / 2) : Grey(128); }
} // namespace vb

class Ising : public Bitmap<int> {
public:
    Ising(const Hub &H, size_t n, double beta, size_t con_) : Bitmap<int>(H.title, {n, n}), con(con_), p(2 * size_t(con) + 1) {
        for (auto z : coo_range(size)) put(z, prng.bernoulli(.5) ? 1 : -1);
        if (con == 6) {
            size_t m = 2 * n / 3;
            for (size_t ii = 0; ii < n; ++ii)
                for (size_t jj = 0; jj < n; ++jj)
                    if ((jj > m) || (ii < jj / 2) || (ii > m + jj / 2)) put({ii, jj}, 0);
        }
        for (size_t c = 0; c <= 2 * con; ++c) p[c] = 1 / (1 + exp(-beta * (int(c) - int(con))));
    };

    void run() {
        while (visible()) {
            auto z = prng.uniform_coo(size);
            if (at(z) == 0) continue;
            int c = 0;
            for (size_t d = 0; d < con; ++d) c += atp(coo(z) + dz[d]);
            put(z, prng.bernoulli(p[size_t(c + int64_t(con))]) ? 1 : -1);
        }
    }

    void output_pdf(const Hub &H, const std::string &s = "") {
        Figure      F{H.title};
        cpx         shift;
        vector<cpx> pattern;
        if (con == 6) {
            shift = {-.5, sqrt(3.0) / 2};
            for (int k = 0; k < 6; ++k) pattern.push_back(exp(cpx(0, (2 * k + 1) * M_PI / 6)) / sqrt(3.0));
        } else {
            shift   = I;
            pattern = {{.5, .5}, {-.5, .5}, {-.5, -.5}, {.5, -.5}};
        }
        for (const auto &z : coo_range(size)) {
            Color c = to_Color(at(z));
            if (c == to_Color(0)) continue;
            vector<cpx> path;
            path.reserve(pattern.size());
            for (auto zz : pattern) path.push_back(cpx(double(z.x)) + cpx(double(z.y)) * shift + zz);
            F.add(make_unique<Polygon>(path, Pen(BLACK, 1, c, true)));
        }
        F.output_pdf(s);
    }

    void explore() {
        ucoo   z0{size.x / 2, (con == 6) ? size.y / 3 : size.y / 2};
        vector list{z0};
        auto   s = at(list.back());
        while (!list.empty()) {
            auto z = list.back();
            list.pop_back();
            if (at(z) == 3) continue;
            put(z, 3);
            for (unsigned d = 0; d < con; ++d)
                if (auto nz = z + dz[d]; fits(nz) && (at(nz) == s)) list.push_back(nz);
        }
    }

    size_t         con;
    vector<double> p;
};

auto main(int argc, char **argv) -> int {
    Hub   H("2D Ising model", argc, argv, "n=50,b=.7,c=4");
    Ising conf(H, H['n'], H['b'], H['c']);
    conf.show();
    conf.run();
    conf.explore();
    conf.output_pdf(H);
}
