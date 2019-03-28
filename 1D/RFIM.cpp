#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Random field Ising model", argc, argv, "n=5,s=0");
    size_t n = H['n'];
    prng.seed(size_t(H['s']));

    size_t nconfig = 1;
    for (size_t i = 0; i < n; ++i) nconfig *= 3;
    vector<int>    config(n, 0);
    vector<double> field(n);
    vector<double> Z(2 * n + 1, 0.0);

    for (size_t i = 0; i < n; ++i) field[i] = (prng.gaussian() > 0 ? 1 : -1);

    for (size_t c = 0; c < nconfig; ++c) {
        double ff = 0;
        size_t n0 = 0, n1 = 0, n2 = 0;

        for (size_t i = 0; i < n; ++i) {
            if (config[i] == 0) {
                ++n0;
                ff += field[i];
            }
            if (config[i] == 1) {
                ++n1;
                ff -= .5 * field[i];
            }
            if (config[i] == 2) {
                ++n2;
                ff -= .5 * field[i];
            }
        }

        Z[n + n1 - n2] += exp(n * ff - 3 * double(n0 * n1 + n1 * n2 + n2 * n0));

        ++config[0];
        for (size_t i = 0; (config[i] == 3) && (i < n - 1); ++i) {
            config[i] = 0;
            ++config[i + 1];
        }
    }

    for (size_t i = 0; i < 2 * n + 1; ++i) cout << int(i) - int(n) << " " << -log(Z[i]) << endl;

    return 0;
}
