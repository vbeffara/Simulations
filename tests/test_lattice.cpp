#include <iomanip>
#include <spdlog/spdlog.h>
#include <vb/Figure.h>
#include <vb/Lattice.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

Lattice G() {
    Lattice G(12, cpx(0, sqrt(6.0 / 7)));

    G.bond(0, 1).bond(1, 0, {1, 0}).bond(0, 2).bond(0, 11).bond(0, 3).bond(1, 3).bond(1, 4).bond(4, 0, {1, 0});
    G.bond(2, 11).bond(8, 11).bond(5, 11).bond(5, 8).bond(6, 11).bond(5, 6).bond(6, 8).bond(3, 11).bond(3, 6);
    G.bond(3, 7).bond(3, 4).bond(6, 7).bond(6, 9).bond(4, 7).bond(4, 10).bond(4, 2, {1, 0}).bond(10, 2, {1, 0});
    G.bond(2, 8).bond(2, 0, {0, 1}).bond(8, 0, {0, 1}).bond(8, 9).bond(9, 0, {0, 1}).bond(7, 9).bond(7, 10);
    G.bond(9, 10).bond(9, 1, {0, 1}).bond(10, 1, {0, 1}).bond(10, 0, {1, 1});
    return G;
}

int main(int argc, char **argv) {
    Hub     H("Testing Lattice", argc, argv);
    Lattice L = SV();
    L.relax(1e-14);
    L.tau = L.tau_rw();
    L.optimize(cost_cp);
    spdlog::info("Modulus: tau = {}", L.tau);

    Pen p(Color(255, 0, 0), 1, Color(255, 255, 0));

    Figure F{H.title};

    vector<cpx> fd;
    fd.push_back(L({1, 1}));
    fd.push_back(L({2, 1}));
    fd.push_back(L({2, 2}));
    fd.push_back(L({1, 2}));
    F.add(std::make_unique<Polygon>(fd, Pen(BLACK, 0, Grey(200), true)));

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < L.n; ++k) F.add(std::make_unique<Circle>(L({i, j}, k), L.r[k], Pen(BLACK, .2)));

    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            for (int k = 0; k < L.n; ++k)
                for (unsigned l = 0; l < L.adj[k].size(); ++l)
                    F.add(std::make_unique<Segment>(L({i, j}, k), L({i, j}, k) + L.shift(k, l), Pen(BLACK, .1)));

    F.show();
    F.pause();
    F.output_pdf(H.title);
}
