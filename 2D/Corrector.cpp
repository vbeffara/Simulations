#include <vb/Map.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

int center   = 6;
int infinity = 13;

auto main(int argc, char **argv) -> int {
    Hub    H("Corrector", argc, argv, "n=10,p=.9,b");
    size_t n     = H['n'];
    double p     = H['p'];
    bool   batch = H['b'];

    Map             m(H.title, n * n);
    vector<uint8_t> adj(n * n, 0);

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            auto ij = i + n * j;

            if (prng.bernoulli(p)) adj[ij] |= 1U;
            if (prng.bernoulli(p)) adj[ij] |= 2U;
            if ((i > 0) && ((adj[ij - 1] & 1U) != 0)) adj[ij] |= 4U;
            if ((j > 0) && ((adj[ij - n] & 2U) != 0)) adj[ij] |= 8U;
        }
    }

    for (size_t i = 0; i < n; ++i) {
        adj[i] |= 5U;
        adj[n * i] |= 10U;
        adj[n - 1 + n * i] |= 10U;
        adj[(n - 1) * n + i] |= 5U;
    }

    for (size_t i = 0; i < n; ++i) {
        adj[i] &= 7U;
        adj[n * i] &= 11U;
        adj[n - 1 + n * i] &= 14U;
        adj[(n - 1) * n + i] &= 13U;
    }

    for (size_t i = 0; i < n; ++i) {
        for (size_t j = 0; j < n; ++j) {
            auto ij = i + n * j;

            if ((i < n - 1) && ((adj[ij] & 1U) != 0)) m << Edge(ij, ij + 1);
            if ((j < n - 1) && ((adj[ij] & 2U) != 0)) m << Edge(ij, ij + n);
            if ((i > 0) && ((adj[ij] & 4U) != 0)) m << Edge(ij, ij - 1);
            if ((j > 0) && ((adj[ij] & 8U) != 0)) m << Edge(ij, ij - n);

            m.v[ij]->z = cpx(i, j);

            m.bd[ij] = ((i == 0) || (i == n - 1) || (j == 0) || (j == n - 1));
        }
    }

    if (!batch) {
        m.show();
        m.pause();
    }

    double output = m.balance();
    cout << n << " " << p << " " << output << endl;

    if (!batch) m.pause();
}
