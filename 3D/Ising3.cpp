#include <vb/Cube.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

class Ising3 : public vb::Cube {
public:
    int                 b;
    bool                k;
    double              beta;
    std::vector<double> glaub, kaw;
    Ising3(const vb::Hub &H, int n, bool k_, double b_) : Cube(H.title, {n, n, n}), b(0), k(k_), beta(b_) {
        for (int k = 0; k <= 6; ++k) glaub.push_back(exp(k * beta) / (exp(k * beta) + exp((6 - k) * beta)));
        for (int k = 0; k <= 12; ++k) kaw.push_back(1 / (1 + exp(2 * beta * (k - 6))));
    };

    int64_t nbsum(vb::coo3 c) {
        int S = 0;
        for (int i = 0; i < 6; ++i) S += atp(c + vb::dz3[i]) != 0 ? 1 : 0;
        return S;
    }

    void spin(vb::coo3 c) {
        if (k) {
            int      d  = vb::prng.uniform_int(6);
            vb::coo3 z2 = c + vb::dz3[d];
            if (at(c) == atp(z2)) return;
            if (vb::prng.bernoulli(kaw[size_t(nbsum(z2) - nbsum(c) + 6)]))
                put(c, 255), putp(z2, 0);
            else
                put(c, 0), putp(z2, 255);
        } else {
            put(c, vb::prng.bernoulli(glaub[size_t(nbsum(c))]) ? 255 : 0);
        }
    }

    void swipe() {
        for (int64_t t = 0; t < size.x * size.y * size.z; ++t) {
            vb::coo3 c = vb::prng.uniform_coo3(size, b);
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
                    const vb::coo3 &cc = c;
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
            for (int64_t i = 0; i < I.size.x; ++i)
                for (int64_t j = 0; j < I.size.x; ++j) {
                    I.put({i, j, I.size.z - 1}, 255);
                    I.put({0, i, j}, 255);
                    I.put({i, 0, j}, 255);
                }
        });
        emplace("cube", [](Ising3 &I) {
            I.b = 0;
            for (int64_t x = I.size.x / 10; x < 9 * I.size.x / 10; ++x)
                for (int64_t y = I.size.y / 10; y < 9 * I.size.y / 10; ++y)
                    for (int64_t z = I.size.z / 10; z < 9 * I.size.z / 10; ++z) I.put({x, y, z}, 255);
        });
        emplace("slope", [&H](Ising3 &I) {
            I.b = 1;
            for (auto c = I.begin(); c != I.end(); ++c)
                if ((c.y - I.size.y * .5) - double(H['p']) * (c.z - I.size.z * .5) + double(H['q']) * (c.x - I.size.x * .5) < 0)
                    I.put(c, 255);
        });
    }
};

int main(int argc, char **argv) {
    vb::Hub H("3D Ising model", argc, argv, "n=50,b=1,t=0,p=.5,q=0,k,c=bernoulli,s=0,m,i,r=0");
    if (size_t r = H['r']) vb::prng.seed(r);
    Ising3 C(H, H['n'], H['k'], H['b']);
    BCs{H}[H['c']](C);
    C.show();
    size_t T = H['t'];
    if (T == 0) T = 2 * size_t(H['n']);
    {
        vb::ProgressBar P(T);
        size_t          s = H['s'];
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
