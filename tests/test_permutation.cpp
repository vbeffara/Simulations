#include <spdlog/spdlog.h>
#include <vb/math/Permutation.h>
#include <vb/util/Hub.h>
#include <vb/util/Stream.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub H("Streams and permutations", argc, argv, "n=10");
    for (size_t k = 1; k <= size_t(H['n']); ++k) {
        spdlog::info("n: {:>2}  permutations: {:>7}  partitions: {:>2}", k, size(permutations(k)), size(partitions(k)));
    }
}
