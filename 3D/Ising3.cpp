#include <map>
#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

class Ising3 : public vb::Cube {
public:
    size_t              b{0};
    bool                k;
    double              beta;
    std::vector<double> glaub, kaw;

    Ising3(const std::string &title, size_t n, bool k_, double b_) : Cube(title, {n, n, n}), k(k_), beta(b_) {
        for (int kk = 0; kk <= 6; ++kk) glaub.push_back(exp(kk * beta) / (exp(kk * beta) + exp((6 - kk) * beta)));
        for (int kk = 0; kk <= 12; ++kk) kaw.push_back(1 / (1 + exp(2 * beta * (kk - 6))));
    };

    auto nbsum(vb::coo3 c) -> int64_t {
        int S = 0;
        for (const auto &dz : vb::dz3) S += atp(c + dz) != 0 ? 1 : 0;
        return S;
    }

    void spin(vb::ucoo3 c) {
        if (k) {
            auto d  = vb::prng.uniform_int(6U);
            auto z2 = vb::coo3(c) + vb::dz3[d];
            if (at(c) == atp(z2)) return;
            if (vb::prng.bernoulli(kaw[size_t(nbsum(z2) - nbsum(vb::coo3(c)) + 6)]))
                put(c, 255), putp(z2, 0);
            else
                put(c, 0), putp(z2, 255);
        } else {
            put(c, vb::prng.bernoulli(glaub[size_t(nbsum(vb::coo3(c)))]) ? 255 : 0);
        }
    }

    void swipe() {
        for (size_t t = 0; t < size.x * size.y * size.z; ++t) {
            auto c = vb::prng.uniform_coo3(size, b);
            spin(c);
        }
    }
};

class BCs : public std::map<std::string, std::function<void(Ising3 &)>> {
public:
    BCs(double p, double q) {
        emplace("bernoulli", [p](Ising3 &I) {
            I.b = 0;
            for (auto c = I.begin(); c != I.end(); ++c)
                if (vb::prng.bernoulli(p)) {
                    const vb::ucoo3 &cc = c;
                    I.put(cc, 255);
                }
        });
        emplace("dobrushin", [](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if (c.y < I.size.y / 2) I.put(c, 255);
        });
        emplace("step", [](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if ((c.y < I.size.y / 2) || ((c.y == I.size.y / 2) && (c.z > I.size.z / 2))) I.put(c, 255);
        });
        emplace("111", [](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if (c.x + c.y + (I.size.z - c.z) < (I.size.x + I.size.y + I.size.z) / 2) I.put(c, 255);
        });
        emplace("hexagon", [](Ising3 &I) {
            I.b = 1;
            for (size_t i = 0; i < I.size.x; ++i)
                for (size_t j = 0; j < I.size.x; ++j) {
                    I.put({i, j, I.size.z - 1}, 255);
                    I.put({0, i, j}, 255);
                    I.put({i, 0, j}, 255);
                }
        });
        emplace("cube", [](Ising3 &I) {
            I.b = 0;
            for (size_t x = I.size.x / 10; x < 9 * I.size.x / 10; ++x)
                for (size_t y = I.size.y / 10; y < 9 * I.size.y / 10; ++y)
                    for (size_t z = I.size.z / 10; z < 9 * I.size.z / 10; ++z) I.put({x, y, z}, 255);
        });
        emplace("slope", [p, q](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if ((double(c.y) - double(I.size.y) * .5) - p * (double(c.z) - double(I.size.z) * .5) +
                        q * (double(c.x) - double(I.size.x) * .5) <
                    0)
                    I.put(c, 255);
        });
    }
};

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "3D Ising model");
    auto n = clp.param("n", size_t(50), "Grid size");
    auto b = clp.param("b", 1.0, "Inverse temperature");
    auto t = clp.param("t", size_t(0), "Number of sweeps");
    auto p = clp.param("p", 0.5, "Probability / slope p");
    auto q = clp.param("q", 0.0, "Slope q");
    auto k = clp.flag("k", "Use Kawasaki dynamics");
    auto c = clp.param("c", std::string("bernoulli"), "Boundary condition");
    auto s = clp.param("s", size_t(0), "Number of snapshots");
    auto m = clp.flag("m", "Disable mirrors");
    auto i = clp.flag("i", "Dim upper half");
    auto r = clp.param("r", size_t(0), "Random seed");
    clp.finalize();
    if (r != 0) vb::prng.seed(r);
    Ising3 C(clp.title, n, k, b);
    BCs{p, q}[c](C);
    C.show();
    size_t T = t;
    if (T == 0) T = 2 * n;
    {
        vb::ProgressBar P(T);
        for (size_t tt = 0; tt < T; ++tt) {
            if ((s > 0) && (tt % (T / s)) == 0) C.output_pov(fmt::format("snapshots/snapshot_{:04d}", tt / (T / s)));
            C.swipe();
            P.set(tt);
        }
    }
    if (m) C.mirrors = false;
    if (i)
        for (auto cc = C.begin(); cc != C.end(); ++cc) C.put(cc, (cc.y > C.size.y / 2) ? C.at(cc) / 2 : C.at(cc));

    C.output_pov(clp.title);
}
