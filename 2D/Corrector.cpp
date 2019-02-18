#include <vb/Map.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

int center   = 6;
int infinity = 13;

int main(int argc, char **argv) {
    Hub    H("Corrector", argc, argv, "n=10,p=.9,b");
    int    n     = H['n'];
    double p     = H['p'];
    bool   batch = H['b'];

    Map             m(H.title, n * n);
    vector<uint8_t> adj(n * n, 0);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int ij = i + n * j;

            if (prng.bernoulli(p)) adj[ij] |= 1u;
            if (prng.bernoulli(p)) adj[ij] |= 2u;
            if ((i > 0) && ((adj[ij - 1] & 1u) != 0)) adj[ij] |= 4u;
            if ((j > 0) && ((adj[ij - n] & 2u) != 0)) adj[ij] |= 8u;
        }
    }

    for (int i = 0; i < n; ++i) {
        adj[i] |= 5u;
        adj[n * i] |= 10u;
        adj[n - 1 + n * i] |= 10u;
        adj[(n - 1) * n + i] |= 5u;
    }

    for (int i = 0; i < n; ++i) {
        adj[i] &= 7u;
        adj[n * i] &= 11u;
        adj[n - 1 + n * i] &= 14u;
        adj[(n - 1) * n + i] &= 13u;
    }

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            int ij = i + n * j;

            if ((i < n - 1) && ((adj[ij] & 1u) != 0)) m << Edge(ij, ij + 1);
            if ((j < n - 1) && ((adj[ij] & 2u) != 0)) m << Edge(ij, ij + n);
            if ((i > 0) && ((adj[ij] & 4u) != 0)) m << Edge(ij, ij - 1);
            if ((j > 0) && ((adj[ij] & 8u) != 0)) m << Edge(ij, ij - n);

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
