#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using std::vector, vb::prng, vb::BLACK, vb::WHITE, vb::coo;

vector<bool> init_ok_none() {
    vector<bool> ok(256, false);
    for (size_t i = 0; i < 256; i++) ok[i] = true;
    return ok;
}

vector<bool> init_ok_glass() {
    // Flippable iff at least two of the 4 neighbors are empty.
    vector<bool> ok(256, false);
    for (unsigned i = 0; i < 256; i++) {
        int tmp = 0;
        if ((i & 1U) != 0) tmp++;
        if ((i & 4U) != 0) tmp++;
        if ((i & 16U) != 0) tmp++;
        if ((i & 64U) != 0) tmp++;
        ok[i] = (tmp <= 2);
    }
    return ok;
}

vector<bool> init_ok_connect4() {
    /* This is connectivity-conditioning, on  the square lattice (which is
     * a bit awkward, in particular it is not ergodic for the wrong reason
     * that the lattice  and dual lattice look different  - the checkboard
     * configuration is fixed ...). */

    vector<bool>     ok(256, false);
    vector<unsigned> tmp1(8), tmp2(8);

    for (unsigned i = 0; i < 256; i++) {
        for (unsigned j = 0; j < 8; j++) {
            tmp1[j] = (i & (1U << j)) / (1U << j);
            tmp2[j] = 1U << j;
        }

        if ((tmp1[0] == tmp1[2]) && (tmp1[0] == tmp1[1])) tmp2[2] = tmp2[0];
        if ((tmp1[2] == tmp1[4]) && (tmp1[2] == tmp1[3])) tmp2[4] = tmp2[2];
        if ((tmp1[4] == tmp1[6]) && (tmp1[4] == tmp1[5])) tmp2[6] = tmp2[4];
        if ((tmp1[6] == tmp1[0]) && (tmp1[6] == tmp1[7])) tmp2[0] = tmp2[6];
        if ((tmp1[0] == tmp1[2]) && (tmp1[0] == tmp1[1])) tmp2[2] = tmp2[0];
        if ((tmp1[2] == tmp1[4]) && (tmp1[2] == tmp1[3])) tmp2[4] = tmp2[2];

        unsigned nb1 = (tmp1[0] * tmp2[0]) | (tmp1[2] * tmp2[2]) | (tmp1[4] * tmp2[4]) | (tmp1[6] * tmp2[6]);
        unsigned nb2 = ((1 - tmp1[0]) * tmp2[0]) | ((1 - tmp1[2]) * tmp2[2]) | ((1 - tmp1[4]) * tmp2[4]) | ((1 - tmp1[6]) * tmp2[6]);

        nb1 = (nb1 & 1U) + ((nb1 & 4U) / 4) + ((nb1 & 16U) / 16) + ((nb1 & 64U) / 64);
        nb2 = (nb2 & 1U) + ((nb2 & 4U) / 4) + ((nb2 & 16U) / 16) + ((nb2 & 64U) / 64);

        ok[i] = (nb1 <= 1) && (nb2 <= 1);
    }
    return ok;
}

vector<bool> init_ok_connect6() {
    // Connectivity-conditioning on the triangular lattice.

    vector<bool>     ok(256, false);
    vector<unsigned> tmp(6, 0);

    for (unsigned i = 0; i < 256; i++) {
        tmp[0] = (i & 1U) / 1;
        tmp[1] = (i & 2U) / 2;
        tmp[2] = (i & 4U) / 4;
        tmp[3] = (i & 16U) / 16;
        tmp[4] = (i & 32U) / 32;
        tmp[5] = (i & 64U) / 64;

        int nb = 0;
        if (tmp[1] != tmp[0]) nb++;
        if (tmp[2] != tmp[1]) nb++;
        if (tmp[3] != tmp[2]) nb++;
        if (tmp[4] != tmp[3]) nb++;
        if (tmp[5] != tmp[4]) nb++;
        if (tmp[0] != tmp[5]) nb++;

        ok[i] = (nb <= 2);
    }
    return ok;
}

class Glass : public vb::Image {
public:
    Glass(const vb::Hub &H, size_t n) : vb::Image(H.title, {n, n}) {
        std::map<std::string, std::function<vector<bool>()>> init;
        init.emplace("none", init_ok_none);
        init.emplace("glass", init_ok_glass);
        init.emplace("connect4", init_ok_connect4);
        init.emplace("connect6", init_ok_connect6);
        ok = init[H['c']]();

        fill({0, 0}, BLACK);
        for (size_t i = 0; i < n; i++) put({i, n / 2}, WHITE);
        show();
    };

    void run(const vb::Hub &H) {
        auto   n = size.x;
        double p = H['p'];

        for (size_t i = 0; i < 2000 * n * n; i++) {
            // TODO: uniform_coo
            size_t x = 1 + prng.uniform_int(n - 2), y = 1 + prng.uniform_int(n - 2), nb = 0;
            if (at({x + 1, y}) != BLACK) nb += 1;
            if (at({x + 1, y + 1}) != BLACK) nb += 2;
            if (at({x, y + 1}) != BLACK) nb += 4;
            if (at({x - 1, y + 1}) != BLACK) nb += 8;
            if (at({x - 1, y}) != BLACK) nb += 16;
            if (at({x - 1, y - 1}) != BLACK) nb += 32;
            if (at({x, y - 1}) != BLACK) nb += 64;
            if (at({x + 1, y - 1}) != BLACK) nb += 128;

            if (ok[nb]) {
                if (prng.bernoulli(p)) {
                    if (at({x, y}) == BLACK) {
                        uint8_t tmp = uint8_t(at({x + 1, y}));
                        tmp |= uint8_t(at({x - 1, y}));
                        tmp |= uint8_t(at({x, y + 1}));
                        tmp |= uint8_t(at({x, y - 1}));
                        if (tmp == 0) tmp = 85;
                        put({x, y}, vb::Grey(tmp));
                    }
                } else
                    put({x, y}, BLACK);
            }
        }
    }

    vector<bool> ok;
};

int main(int argc, char **argv) {
    vb::Hub H("Glassy Glauber dynamics for percolation", argc, argv, "n=300,p=.5,c=none");
    Glass   img(H, H['n']);
    img.run(H);
}
