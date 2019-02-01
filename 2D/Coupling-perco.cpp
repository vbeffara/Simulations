#include <vb/Bitmap.h>

using namespace vb;
using namespace std;

class Configuration : public Image {
public:
    explicit Configuration(int n) : Image(n, n), expl({n, n}), table(2 * n * n + 1) {}

    void compute_cpts(int r1) {
        int t = 0;
        for (int x = 0; x < w(); x++) {
            for (int y = 0; y < h(); y++) {
                coo z{x, y};
                expl[z] = (at(z) == WHITE ? 1 : -1) * (++t);
            }
        }

        for (int x = -r1 + 1; x < r1 - 1; x++)
            for (int y = -r1 + 1; y < r1 - 1; y++) expl[{x + w() / 2, y + h() / 2}] = 0;

        bool dirty = true;
        while (dirty) {
            dirty = false;
            for (int x = 0; x < w(); x++) {
                for (int y = 0; y < h(); y++) {
                    coo z{x, y};
                    for (int i = 0; i < 6; ++i) {
                        coo zz = z + dz[i];
                        if (!expl.contains(zz)) continue;
                        if ((expl[z] > 0) && (expl[zz] > expl[z])) {
                            expl[z] = expl[zz];
                            dirty   = true;
                        }
                        if ((expl[z] < 0) && (expl[zz] < expl[z])) {
                            expl[z] = expl[zz];
                            dirty   = true;
                        }
                    }
                }
            }
        }
    }

    int nbarms(int r1, int r2, unsigned sides) {
        compute_cpts(r1);
        int N = w();

        for (auto &t : table) t = 0;
        for (int i = -r2; i < r2; i++) {
            if ((sides & 1u) != 0) table[N * N + expl[{N / 2 + i, N / 2 - r2}]] = 1;
            if ((sides & 2u) != 0) table[N * N + expl[{N / 2 + i, N / 2 + r2 - 1}]] = 1;
            if ((sides & 4u) != 0) table[N * N + expl[{N / 2 - r2, N / 2 + i}]] = 1;
            if ((sides & 8u) != 0) table[N * N + expl[{N / 2 + r2 - 1, N / 2 + i}]] = 1;
        }

        int n = 0;
        int k;
        for (int i = -r1; i < r1; i++) {
            k = expl[{N / 2 + i, N / 2 - r1}];
            if (table[N * N + k] == 1) {
                table[N * N + k] = 0;
                n++;
            }
            k = expl[{N / 2 + i, N / 2 + r1 - 1}];
            if (table[N * N + k] == 1) {
                table[N * N + k] = 0;
                n++;
            }
            k = expl[{N / 2 - r1, N / 2 + i}];
            if (table[N * N + k] == 1) {
                table[N * N + k] = 0;
                n++;
            }
            k = expl[{N / 2 + r1 - 1, N / 2 + i}];
            if (table[N * N + k] == 1) {
                table[N * N + k] = 0;
                n++;
            }
        }

        return n;
    }

    bool test1(int r1, int r2, int r3) { return (nbarms(r1, r3, 15) == 4) && (nbarms(r2, r3, 15) == 4) && (nbarms(r1, r2, 15) == 4); }

    bool test2(int r1, int r2, int r3) { return test1(r1, r2, r3) && (nbarms(r1, r3, 6) == 2); }

    void pick(int r1, int r2, int r3) {
        int n = 0;
        while (true) {
            cerr << ++n << " \r";
            for (int x = 0; x < w(); x++)
                for (int y = 0; y < h(); y++) put({x, y}, prng.bernoulli(.5) ? WHITE : BLACK);

            for (int x = -r1 + 1; x < r1 - 1; x++) {
                for (int y = -r1 + 1; y < r1 - 1; y++) {
                    put({x + w() / 2, y + h() / 2}, BLACK);
                    put({x + w() / 2, y + h() / 2}, BLACK);
                }
            }

            if (test2(r1, r2, r3)) break;
        }
        cerr << endl;
    }

    Array<int>   expl;
    vector<char> table;
};

class Coupling : public Image {
public:
    explicit Coupling(int r) : Image(2 * r, 2 * r), r1(r / 4), r2(r / 2), r3(r), c1(2 * r), c2(2 * r) {
        c1.pick(r1, r2, r3);
        for (int i = 0; i < w(); ++i)
            for (int j = 0; j < h(); ++j) c2[{i, j}] = c1[{i, j}];
        c1.show();
        c2.show();
        show();
        compute_diff();
        update();
    }

    int compute_diff() {
        int n = 0;
        for (int i = 0; i < c1.w(); i++) {
            for (int j = 0; j < c1.h(); j++) {
                coo z{i, j};
                if (c1.at(z) == c2.at(z)) {
                    put(z, BLACK);
                } else if (int(c1.at(z)) > int(c2.at(z))) {
                    put(z, GREEN);
                    n++;
                } else {
                    put(z, RED);
                    n++;
                }
            }
        }
        return n;
    }

    void run() {
        while (true) {
            coo z{0, 0};
            while (true) {
                z     = prng.uniform_coo(size);
                int x = z.x, y = z.y;
                if ((x < w() / 2 - r1) || (x >= w() / 2 + r1) || (y < h() / 2 - r1) || (y >= h() / 2 + r1)) break;
            }

            auto v = prng.bernoulli(.5) ? WHITE : BLACK;
            {
                auto old = c1.at(z);
                c1.put(z, v);
                if (!c1.test1(r1, r2, r3)) c1.put(z, old);
            }
            {
                auto old = c2.at(z);
                c2.put(z, v);
                if (!c2.test2(r1, r2, r3)) c2.put(z, old);
            }

            cerr << compute_diff() << " \r";
            update();
        }
    }

    int           r1, r2, r3;
    Configuration c1, c2;
};

int main(int argc, char **argv) {
    H.init("Coupling percolation configurations", argc, argv, "r=20");
    Coupling C(H['r']);
    C.run();
}
