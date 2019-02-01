#include <vb/Hub.h>
#include <vb/util/PRNG.h>

using vb::H;

int main(int argc, char **argv) {
    H.init("Rarefaction fan", argc, argv, "n=20");
    int n = H['n'];

    std::vector<int> field(2 * n);
    for (int i = 0; i < 2 * n; i++) field[i] = n - i;

    while (true) {
        int i = vb::prng.uniform_int(2 * n - 1);
        if (field[i] > field[i + 1]) std::swap(field[i], field[i + 1]);
        if ((field[0] < 0) || (field[2 * n - 1] > 0)) break;
    }

    for (int i = 0; i < 2 * n; i++) std::cout << i << " " << field[i] << std::endl;
}
