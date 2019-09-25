#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using vb::Hub;

auto main(int argc, char **argv) -> int {
    Hub    H("Rarefaction fan", argc, argv, "n=20");
    size_t n = H['n'];

    std::vector<int> field(2 * n);
    for (size_t i = 0; i < 2 * n; i++) field[i] = int(n) - int(i);

    while (true) {
        auto i = vb::prng.uniform_int(2 * n - 1);
        if (field[i] > field[i + 1]) std::swap(field[i], field[i + 1]);
        if ((field[0] < 0) || (field[2 * n - 1] > 0)) break;
    }

    for (size_t i = 0; i < 2 * n; i++) std::cout << i << " " << field[i] << std::endl;
}
