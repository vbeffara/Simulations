#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

class Ising3 : public vb::Cube {
public:
    size_t              b{0};
    bool                k;
    double              beta;
    std::vector<double> glaub, kaw;

    Ising3(const vb::Hub &H, size_t n, bool k_, double b_) : Cube(H.title, {n, n, n}), k(k_), beta(b_) {
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
    explicit BCs(const vb::Hub &H) {
        emplace("bernoulli", [&H](Ising3 &I) {
            I.b = 0;
            for (auto c = I.begin(); c != I.end(); ++c)
                if (vb::prng.bernoulli(H['p'])) {
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
        emplace("slope", [&H](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if ((double(c.y) - double(I.size.y) * .5) - double(H['p']) * (double(c.z) - double(I.size.z) * .5) +
                        double(H['q']) * (double(c.x) - double(I.size.x) * .5) <
                    0)
                    I.put(c, 255);
        });
    }
};

auto main(int argc, char **argv) -> int {
    vb::Hub const H("3D Ising model", argc, argv, "n=50,b=1,t=0,p=.5,q=0,k,c=bernoulli,s=0,m,i,r=0");
    if (size_t const r = H['r']) vb::prng.seed(r);
    Ising3 C(H, H['n'], H['k'], H['b']);
    BCs{H}[H['c']](C);
    C.show();
    size_t T = H['t'];
    if (T == 0) T = 2 * size_t(H['n']);
    {
        vb::ProgressBar P(T);
        size_t const    s = H['s'];
        for (size_t t = 0; t < T; ++t) {
            if ((s > 0) && (t % (T / s)) == 0) C.output_pov(fmt::format("snapshots/snapshot_{:04d}", t / (T / s)));
            C.swipe();
            P.set(t);
        }
    }
    if (H['m']) C.mirrors = false;
    if (H['i'])
        for (auto c = C.begin(); c != C.end(); ++c) C.put(c, (c.y > C.size.y / 2) ? C.at(c) / 2 : C.at(c));

    C.output_pov(H.title);
}
