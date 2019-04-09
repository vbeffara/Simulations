#include <vb/Bitmap.h>
#include <vb/Figure.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> Color to_Color(int t) { return t != 0 ? Indexed((3 + t) / 2) : Grey(128); }
} // namespace vb

class Ising : public Bitmap<int> {
public:
    Ising(const Hub &H, size_t n, double beta, size_t con) : Bitmap<int>(H.title, {n, n}), con(con), p(2 * size_t(con) + 1) {
        for (auto z : coo_range(size)) put(z, prng.bernoulli(.5) ? 1 : -1);
        if (con == 6) {
            size_t m = 2 * n / 3;
            for (size_t i = 0; i < n; ++i)
                for (size_t j = 0; j < n; ++j)
                    if ((j > m) || (i < j / 2) || (i > m + j / 2)) put({i, j}, 0);
        }
        for (size_t c = 0; c <= 2 * con; ++c) p[c] = 1 / (1 + exp(-beta * (int(c) - int(con))));
    };

    void run() {
        while (visible()) {
            auto z = prng.uniform_coo(size);
            if (at(z) == 0) continue;
            int c = 0;
            for (size_t d = 0; d < con; ++d) c += atp(coo(z) + dz[gsl::index(d)]);
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
        for (size_t i = 0; i < w(); ++i)
            for (size_t j = 0; j < h(); ++j) {
                Color c = to_Color(at({i, j}));
                if (c == to_Color(0)) continue;
                vector<cpx> p;
                p.reserve(pattern.size());
                for (auto zz : pattern) p.push_back(cpx(i) + cpx(j) * shift + zz);
                F.add(make_unique<Polygon>(p, Pen(BLACK, 1, c, true)));
            }
        F.output_pdf(s);
    }

    void explore() {
        ucoo   z0{w() / 2, (con == 6) ? h() / 3 : h() / 2};
        vector list{z0};
        auto   s = at(list.back());
        while (!list.empty()) {
            auto z = list.back();
            list.pop_back();
            if (at(z) == 3) continue;
            put(z, 3);
            for (gsl::index d = 0; d < gsl::index(con); ++d)
                if (auto nz = z + dz[d]; fits(nz) && (at(nz) == s)) list.push_back(nz);
        }
    }

    size_t         con;
    vector<double> p;
};

int main(int argc, char **argv) {
    Hub   H("2D Ising model", argc, argv, "n=50,b=.7,c=4");
    Ising I(H, H['n'], H['b'], H['c']);
    I.show();
    I.run();
    I.explore();
    I.output_pdf(H);
}
