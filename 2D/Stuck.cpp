#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/util/CLP.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

class Stat {
public:
    explicit Stat(int ss = 0) : s(ss), ml(max) { max = std::max(max, s); }
    explicit operator int() { return s; }
    explicit operator Color() {
        if (s < 0) return Color(0, 64, 0);
        if (s == 0) return BLACK;
        if (ml <= .1 * max) return Color(0, 0, 64);
        if (ml <= .9 * max) return Color(0, 0, uint8_t(64 + 128 * (ml - .1 * max) / (.8 * max)));
        return Grey(uint8_t(64 + (s - min) * (255 - 64) / (max - min)));
    }
    int        s, ml;
    static int min, max;
};

int Stat::min = 0, Stat::max = 1;

class Stuck : public Bitmap<Stat> {
public:
    explicit Stuck(double a, double b, bool c, size_t n, bool o, const Hub &H)
        : Bitmap<Stat>(H.title, {2 * n, 2 * n}), alpha(a), beta(b), H(H), c(c), o(o) {
        for (const auto &z : coo_range(size / 2)) at(z * 2) = Stat{-1};
        z = coo(size / 2);
        C.manage(alpha, 0.142, 0.35, "alpha");
        C.lambda<int>([this]() { return nsup(); }, "Support");
        C.lambda<double>([this]() { return -log(double(nsup())) / log(alpha - 1.0 / 7); }, "Prediction");
    }

    void show() override {
        C.show();
        Bitmap<Stat>::show();
    }

    void center() {
        coo c{0, 0};
        for (const auto &z : coo_range(size))
            if (at(z).s == Stat::max) c = coo(z - size / 2);
        c.x -= c.x % 2;
        c.y -= c.y % 2;
        if (c != coo{0, 0}) {
            static Array<Stat> &me  = *this;
            static Array<Stat>  tmp = me;
            for (const auto &z : coo_range(size)) tmp.at(z) = atp(coo(z) + c);
            me = tmp;
            z -= c;
        }
    }

    void run() {
        vector<double> l(4);
        while (visible()) {
            for (unsigned i = 0; i < 4; ++i) {
                coo d = dz[i], dl = dz[(i + 1) % 4], dr = dz[(i + 3) % 4];
                l[i] = alpha * (atp(z + d + d + d).s + atp(z + d + d + dl).s + atp(z + d + d + dr).s) - atp(z + d).s;
            }
            double ml = l[0], sl = 0;
            for (unsigned i = 1; i < 4; ++i) { ml = max(ml, l[i]); }
            for (unsigned i = 0; i < 4; ++i) {
                l[i] = exp(beta * (l[i] - ml));
                sl += l[i];
            }
            for (unsigned i = 0; i < 4; ++i) { l[i] /= sl; }
            coo d = dz[prng.discrete(l)];
            putp(z + d, Stat{atp(z + d).s + 1});
            z += d + d;
        }
        if (o)
            for (size_t x = 0; x < size.x; x += 2) {
                for (size_t y = 1; y < size.y; y += 2) cout << x << " " << y << " " << at({x, y}).s << endl;
                cout << endl;
            }
    }

    void update() override {
        if (c) center();
        int m = Stat::max;
        for (auto z : coo_range(size))
            if (at(z).s > 0) m = min(m, at(z).s);
        if (m < Stat::max) Stat::min = m;
        Bitmap<Stat>::update();
    }

    int nsup() {
        int n = 0;
        for (auto z : coo_range(size))
            if ((at(z).s > 0) && (at(z).ml > .9 * Stat::max)) ++n;
        return n;
    }

    double     alpha, beta;
    coo        z = {0, 0};
    const Hub &H;
    bool       c, o;
    Console    C;
};

int main(int argc, char **argv) {
    CLP  clp(argc, argv, "Stuck walk on the square lattice");
    auto a = clp.param("a", 0.1432, "Reinforcement parameter (alpha)");
    auto b = clp.param("b", 5.0, "Inverse temperature (beta)");
    auto n = clp.param("n", size_t(200), "Domain size");
    auto v = clp.param("v", 0.0, "Snapshot interval time (0 to disable)");
    auto c = clp.flag("c", "Center display on center of mass");
    auto o = clp.flag("o", "Output point coordinates to the console");
    clp.finalize();

    Hub   H("Stuck walk on the square lattice", argc, argv, "");
    Stuck S(a, b, c, n, o, H);
    S.show();
    if (v > 0) S.snapshot_setup("Stuck", v);
    S.run();
}
