#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> auto to_Color(int t) -> Color {
        if (t == 0) return BLACK;
        if (t == 1) return RED;
        return WHITE;
    }
} // namespace vb

class IsingCFTP : public Bitmap<int> {
public:
    explicit IsingCFTP(const Hub &H) : Bitmap<int>(H.title, {H['n'], H['n']}), b(H['b']), d(0), s(H['s']), status(H.title, size) {
        for (const auto &z : coo_range(size)) put(z, 1);
        snap();
        b *= log(1 + sqrt(double(2)));
        for (int i = 0; i <= 4; ++i) p.push_back(exp(b * i) / (exp(b * i) + exp(b * (4 - i))));
    };

    void up(const ucoo z) {
        double   U  = prng.uniform_real();
        unsigned n1 = 0, n2 = 0;
        for (unsigned i = 0; i < 4; ++i) {
            if (atp(coo(z) + dz[i]) >= 1) ++n1;
            if (atp(coo(z) + dz[i]) >= 2) ++n2;
        }
        if (U < p[n2])
            put(z, 2);
        else if (U < p[n1])
            put(z, 1);
        else
            put(z, 0);
    }

    void up() {
        for (size_t i = 0; i < size_t(size.x * size.y); ++i)
            up({d + prng.uniform_int(size.x - 2 * d), d + prng.uniform_int(size.y - 2 * d)});
    }
    void snap() {
        for (auto z : coo_range(size)) status.put(z, at(z));
        status.update();
    }

    void run() {
        vector<string> states;
        states.push_back(prng.state());
        auto n = size.x * size.y;
        while (n > 0) {
            cerr << n << endl;
            for (const auto &z : coo_range(ucoo{d, d}, size - ucoo{d, d})) put(z, 1);
            for (auto t = states.size(); t-- > 0;) {
                prng.state(states[t]);
                for (size_t i = 0; i < (1U << t); ++i) up();
                if (t == states.size() - 1) states.push_back(prng.state());
            }
            snap();
            if (s) status.snapshot();
            n = 0;
            for (auto z : coo_range(size))
                if (at(z) == 1) ++n;
        }
    }

    void bc_0() {
        d = 1;
        for (size_t i = 0; i < size.x; ++i) {
            put({i, 0}, 0);
            put({i, size.y - 1}, 0);
        }
        for (size_t j = 0; j < size.y; ++j) {
            put({0, j}, 0);
            put({size.x - 1, j}, 0);
        }
    }

    void bc_dobrushin() {
        d = 1;
        for (size_t i = 0; i < size.x; ++i) {
            put({i, 0}, 0);
            put({i, size.y - 1}, 2);
        }
        for (size_t j = 0; j < size.y / 2; ++j) {
            put({0, j}, 0);
            put({size.x - 1, j}, 0);
        }
        for (size_t j = size.y / 2; j < size.y; ++j) {
            put({0, j}, 2);
            put({size.x - 1, j}, 2);
        }
    }

    double         b;
    size_t         d;
    bool           s;
    Bitmap<int>    status;
    vector<double> p;
};

auto main(int argc, char *argv[]) -> int {
    Hub       H("CFTP for the Ising model", argc, argv, "n=200,b=1,s");
    IsingCFTP I(H);
    I.bc_dobrushin();
    I.show();
    I.status.show();
    I.run();
    I.pause();
}
