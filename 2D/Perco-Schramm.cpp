/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <vb/Figure.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Perco_Schramm : public Figure {
public:
    explicit Perco_Schramm(const Hub &H)
        : Figure(H.title), w(2 * size_t(H['n'])), h(size_t(H['l']) > 0 ? H['l'] : w - 1), mask(w * h, true) {
        for (size_t ii = 0; ii < w / 2; ++ii) cols.push_back(true);
        for (size_t ii = 0; ii < w / 2; ++ii) cols.push_back(false);
        for (size_t ii = 0; ii < (w - 1) * h; ++ii) cols.push_back(prng.bernoulli(H['p']));
    }

    void tri_boundary() {
        for (size_t j = 0; j < h; ++j) {
            for (size_t ii = 0; ii < w; ++ii) mask[ii + w * j] = (ii <= (w + j) / 2) && (ii >= (w - j) / 2 - 1) && (j < h);
            cols[(w - j) / 2 + w * j - 1] = true;
            cols[(w + j) / 2 + w * j]     = false;
        }
    }

    void rect_boundary() {
        for (size_t j = 0; j < h - 1; ++j) {
            cols[w * j]     = true;
            cols[w * j + h] = false;
        }
    }

    void perc() {
        for (size_t ii = 0; ii < w * h; ++ii)
            if (mask[ii]) {
                cpx const   xy = thepos(ii);
                vector<cpx> coo;
                coo.push_back(xy + cpx(omx, 1));
                coo.push_back(xy + cpx(0, 2));
                coo.push_back(xy + cpx(-omx, 1));
                coo.push_back(xy + cpx(-omx, -1));
                coo.push_back(xy + cpx(0, -2));
                coo.push_back(xy + cpx(omx, -1));
                add(make_unique<Polygon>(coo, Pen(BLACK, 1, cols[ii] ? CYAN : YELLOW, true)));
            }
    }

    void walk() {
        size_t base = w / 2 - 1, dir = 0;
        auto   p = make_unique<Path>(vector<cpx>(0), Pen(BLUE, 4));
        p->z.push_back(thepos(base) + cpx(omx, -1));
        while (base / w < h - 1) {
            seg(p.get(), base, dir, 1);
            auto thenext = follow(base, (dir + 1) % 6);
            if (cols[thenext]) {
                base = thenext;
                dir  = (dir + 5) % 6;
            } else {
                dir = (dir + 1) % 6;
            }
            seg(p.get(), base, dir, 5);
        }
        add(std::move(p));
    }

private:
    size_t       w, h;
    vector<bool> cols, mask;
    const double omx{sqrt(3.0)};

    [[nodiscard]] auto thepos(size_t ii) const -> cpx { return {omx * double(((ii / w) % 2) + 2 * (ii % w)), 3 * double(size_t(ii / w))}; }

    [[nodiscard]] auto follow(size_t base, size_t dir) const -> size_t {
        static const vector<int> fola = {1, int(w), int(w) - 1, -1, -int(w) - 1, -int(w)};
        static const vector<int> folb = {1, int(w) + 1, int(w), -1, -int(w), -int(w) + 1};
        return size_t((((base / w) % 2) != 0 ? folb : fola)[dir] + int64_t(base));
    }

    void seg(Path *p, size_t base, size_t dir, size_t rot) {
        cpx const x1y1 = thepos(base);
        cpx const x2y2 = thepos(follow(base, dir));
        cpx const x3y3 = thepos(follow(base, (dir + rot) % 6));
        p->z.push_back((x1y1 + x2y2 + x3y3) / 3.0);
    }
};

auto main(int argc, char **argv) -> int {
    Hub const     H("Percolation exploration process", argc, argv, "n=28,l=55,p=.5,t");
    Perco_Schramm RS(H);
    if (H['t'])
        RS.tri_boundary();
    else
        RS.rect_boundary();
    RS.perc();
    RS.walk();
    RS.show();
    RS.pause();
    RS.output(H.title);
}
