#include <vb/Bitmap.h>
#include <vb/Ranges.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> Color to_Color(int t) {
        static const vector<Color> C{RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN};
        if ((t >= 0) && (t < 6)) return C[t];
        if (t < 0) return WHITE;
        return Indexed(t);
    }
} // namespace vb

class Potts : public Bitmap<int> {
public:
    Potts(int n, int q, double beta_) : Bitmap<int>(n, n), q(q), beta(beta_) {
        bcs["perio"] = [] {};
        bcs["free"]  = [this] {
            b = 1;
            for (auto z : coos(*this)) put(z, -1);
        };
        bcs["wired"] = [this] {
            b = 1;
            for (int i = 0; i < w(); ++i) {
                put({i, 0}, 1);
                put({i, h() - 1}, 1);
            }
            for (int i = 0; i < h(); ++i) {
                put({0, i}, 1);
                put({w() - 1, i}, 1);
            }
        };
        bcs["tripod"] = [this] {
            b = 1;
            for (int j = 0; j < h() - w() / 2; ++j) {
                for (int i = 0; i < w() / 2; ++i) put({i, j}, 0);
                for (int i = w() / 2; i < w(); ++i) put({i, j}, 2);
            }
            for (int j = h() - w() / 2; j < h(); ++j) {
                for (int i = 0; i < h() - j; ++i) put({i, j}, 0);
                for (int i = h() - j; i < w() - h() + j; ++i) put({i, j}, 1);
                for (int i = w() - h() + j; i < w(); ++i) put({i, j}, 2);
            }
        };
        bcs["quadripod"] = [this] {
            b = 1;
            for (int j = 0; j < h() / 2; ++j) {
                for (int i = 0; i < w() / 2; ++i) put({i, j}, 0);
                for (int i = w() / 2; i < w(); ++i) put({i, j}, 1);
            }
            for (int j = h() / 2; j < h(); ++j) {
                for (int i = 0; i < w() / 2; ++i) put({i, j}, 2);
                for (int i = w() / 2; i < w(); ++i) put({i, j}, 3);
            }
        };
        bcs["quadripod2"] = [this] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h(); ++j) {
                    if (i > j) {
                        if (i + j < w())
                            put({i, j}, 0);
                        else
                            put({i, j}, 1);
                    } else {
                        if (i + j < w())
                            put({i, j}, 2);
                        else
                            put({i, j}, 3);
                    }
                }
        };
        bcs["dobrushin"] = [this] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h() / 2; ++j) put({i, j}, 0);
            for (int i = 0; i < w(); ++i)
                for (int j = h() / 2; j < h(); ++j) put({i, j}, 1);
        };
        bcs["loren"] = [this, q] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h(); ++j)
                    if ((i == 0) || (j == 0) || (i == w() - 1) || (j == h() - 1)) put({i, j}, (q * (i + j) / w()) % q);
        };
        bcs["loren2"] = [this] {
            assert(int(H['q']) >= 6);
            b = 1;
            for (int i = 0; i < w() / 4; ++i) put({i, 0}, 0);
            for (int i = w() / 4; i < 3 * w() / 4; ++i) put({i, 0}, 1);
            for (int i = 3 * w() / 4; i < w(); ++i) put({i, 0}, 2);
            for (int i = 0; i < h() / 2; ++i) put({0, i}, 0);
            for (int i = h() / 2; i < h(); ++i) put({0, i}, 5);
            for (int i = 0; i < h() / 2; ++i) put({w() - 1, i}, 2);
            for (int i = h() / 2; i < h(); ++i) put({w() - 1, i}, 3);
            for (int i = 0; i < w() / 4; ++i) put({i, h() - 1}, 5);
            for (int i = w() / 4; i < 3 * w() / 4; ++i) put({i, h() - 1}, 4);
            for (int i = 3 * w() / 4; i < w(); ++i) put({i, h() - 1}, 3);
        };
        bcs["123"] = [this, q] {
            b     = 1;
            int c = 0;
            for (int i = 0; i < w() - 1; ++i) {
                put({i, 0}, c);
                c = (c + 1) % q;
            }
            for (int i = 0; i < h() - 1; ++i) {
                put({w() - 1, i}, c);
                c = (c + 1) % q;
            }
            for (int i = 0; i < w() - 1; ++i) {
                put({w() - 1 - i, h() - 1}, c);
                c = (c + 1) % q;
            }
            for (int i = 0; i < h() - 1; ++i) {
                put({0, h() - 1 - i}, c);
                c = (c + 1) % q;
            }
        };
        bcs["12123333"] = [this] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h(); ++j) put({i, j}, (j > h() / 2) ? ((i + j) % 2) : 2);
        };
        bcs["1231234444"] = [this] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h(); ++j) put({i, j}, (j > h() / 2) ? ((i + j) % 3) : 3);
        };
        bcs["mostlyfree"] = [this, q] {
            b = 1;
            for (int i = 0; i < w(); ++i)
                for (int j = 0; j < h(); ++j) put({i, j}, q);
        };

        for (int i = 0; i < w(); ++i)
            for (int j = 0; j < h(); ++j) put({i, j}, prng.uniform_int(q));
        beta *= log(1 + sqrt(double(q)));
        bcs[H['c']]();
        show();
    }

    int HH(int i, int j) const { return i == j ? 0 : 1; }
    int HH(const coo & z, int i) const {
        return HH(i, atp(z + dz[0])) + HH(i, atp(z + dz[1])) + HH(i, atp(z + dz[2])) + HH(i, atp(z + dz[3]));
    }

    void up() { up(rand()); }
    void up(coo z) {
        int    i  = prng.uniform_int(q);
        double dH = HH(z, i) - HH(z, atp(z));
        if ((dH <= 0) || prng.bernoulli(exp(-beta * dH))) put(z, i);
    }

    int                           q, b = 0;
    double                        beta;
    map<string, function<void()>> bcs;
};

int main(int argc, char ** argv) {
    H.init("Potts model", argc, argv, "n=500,q=3,b=1,c=free");
    Potts P(H['n'], H['q'], H['b']);
    while (true) P.up();
}
