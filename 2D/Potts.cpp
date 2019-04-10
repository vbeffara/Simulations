#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> Color to_Color(unsigned t) {
        static const vector<Color> C{RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN};
        if ((t >= 0) && (t < 6)) return C[size_t(t)];
        if (t < 0) return WHITE;
        return Indexed(int(t));
    }
} // namespace vb

class Potts : public Bitmap<unsigned> {
public:
    Potts(const Hub &H, size_t n, unsigned q, double beta_) : Bitmap<unsigned>(H.title, {n, n}), q(q), beta(beta_) {
        bcs["perio"] = [] {};
        bcs["free"]  = [this] {
            b = 1;
            for (auto z : coo_range(size)) put(z, unsigned(-1));
        };
        bcs["wired"] = [this] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i) {
                put({i, 0}, 1);
                put({i, size.y - 1}, 1);
            }
            for (size_t i = 0; i < size.y; ++i) {
                put({0, i}, 1);
                put({size.x - 1, i}, 1);
            }
        };
        bcs["tripod"] = [this] {
            b = 1;
            for (size_t j = 0; j < size.y - size.x / 2; ++j) {
                for (size_t i = 0; i < size.x / 2; ++i) put({i, j}, 0);
                for (size_t i = size.x / 2; i < size.x; ++i) put({i, j}, 2);
            }
            for (size_t j = size.y - size.x / 2; j < size.y; ++j) {
                for (size_t i = 0; i < size.y - j; ++i) put({i, j}, 0);
                for (size_t i = size.y - j; i < size.x - size.y + j; ++i) put({i, j}, 1);
                for (size_t i = size.x - size.y + j; i < size.x; ++i) put({i, j}, 2);
            }
        };
        bcs["quadripod"] = [this] {
            b = 1;
            for (size_t j = 0; j < size.y / 2; ++j) {
                for (size_t i = 0; i < size.x / 2; ++i) put({i, j}, 0);
                for (size_t i = size.x / 2; i < size.x; ++i) put({i, j}, 1);
            }
            for (size_t j = size.y / 2; j < size.y; ++j) {
                for (size_t i = 0; i < size.x / 2; ++i) put({i, j}, 2);
                for (size_t i = size.x / 2; i < size.x; ++i) put({i, j}, 3);
            }
        };
        bcs["quadripod2"] = [this] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y; ++j) {
                    if (i > j) {
                        if (i + j < size.x)
                            put({i, j}, 0);
                        else
                            put({i, j}, 1);
                    } else {
                        if (i + j < size.x)
                            put({i, j}, 2);
                        else
                            put({i, j}, 3);
                    }
                }
        };
        bcs["dobrushin"] = [this] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y / 2; ++j) put({i, j}, 0);
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = size.y / 2; j < size.y; ++j) put({i, j}, 1);
        };
        bcs["loren"] = [this, q] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y; ++j)
                    if ((i == 0) || (j == 0) || (i == size.x - 1) || (j == size.y - 1)) put({i, j}, (q * (i + j) / size.x) % q);
        };
        bcs["loren2"] = [this] {
            assert(size_t(H['q']) >= 6);
            b = 1;
            for (size_t i = 0; i < size.x / 4; ++i) put({i, 0}, 0);
            for (size_t i = size.x / 4; i < 3 * size.x / 4; ++i) put({i, 0}, 1);
            for (size_t i = 3 * size.x / 4; i < size.x; ++i) put({i, 0}, 2);
            for (size_t i = 0; i < size.y / 2; ++i) put({0, i}, 0);
            for (size_t i = size.y / 2; i < size.y; ++i) put({0, i}, 5);
            for (size_t i = 0; i < size.y / 2; ++i) put({size.x - 1, i}, 2);
            for (size_t i = size.y / 2; i < size.y; ++i) put({size.x - 1, i}, 3);
            for (size_t i = 0; i < size.x / 4; ++i) put({i, size.y - 1}, 5);
            for (size_t i = size.x / 4; i < 3 * size.x / 4; ++i) put({i, size.y - 1}, 4);
            for (size_t i = 3 * size.x / 4; i < size.x; ++i) put({i, size.y - 1}, 3);
        };
        bcs["123"] = [this, q] {
            b          = 1;
            unsigned c = 0;
            for (size_t i = 0; i < size.x - 1; ++i) {
                put({i, 0}, c);
                c = (c + 1) % q;
            }
            for (size_t i = 0; i < size.y - 1; ++i) {
                put({size.x - 1, i}, c);
                c = (c + 1) % q;
            }
            for (size_t i = 0; i < size.x - 1; ++i) {
                put({size.x - 1 - i, size.y - 1}, c);
                c = (c + 1) % q;
            }
            for (size_t i = 0; i < size.y - 1; ++i) {
                put({0, size.y - 1 - i}, c);
                c = (c + 1) % q;
            }
        };
        bcs["12123333"] = [this] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y; ++j) put({i, j}, (j > size.y / 2) ? ((i + j) % 2) : 2);
        };
        bcs["1231234444"] = [this] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y; ++j) put({i, j}, (j > size.y / 2) ? ((i + j) % 3) : 3);
        };
        bcs["mostlyfree"] = [this, q] {
            b = 1;
            for (size_t i = 0; i < size.x; ++i)
                for (size_t j = 0; j < size.y; ++j) put({i, j}, q);
        };

        for (size_t i = 0; i < size.x; ++i)
            for (size_t j = 0; j < size.y; ++j) put({i, j}, prng.uniform_int(q));
        beta *= log(1 + sqrt(double(q)));
        bcs[H['c']]();
        show();
    }

    [[nodiscard]] unsigned HH(unsigned i, unsigned j) const { return i == j ? 0 : 1; }
    [[nodiscard]] unsigned HH(const coo &z, unsigned i) const {
        return HH(i, atp(z + dz[0])) + HH(i, atp(z + dz[1])) + HH(i, atp(z + dz[2])) + HH(i, atp(z + dz[3]));
    }

    void up() { up(prng.uniform_coo(coo(size))); }
    void up(coo z) {
        auto   i  = prng.uniform_int(q);
        double dH = HH(z, i) - HH(z, atp(z));
        // TODO: at(ucoo()), put(ucoo()) and so on
        if ((dH <= 0) || prng.bernoulli(exp(-beta * dH))) put(ucoo(z), i);
    }

    unsigned                      q, b = 0;
    double                        beta;
    map<string, function<void()>> bcs;
};

int main(int argc, char **argv) {
    Hub   H("Potts model", argc, argv, "n=500,q=3,b=1,c=free");
    Potts P(H, H['n'], H['q'], H['b']);
    while (true) P.up();
}
