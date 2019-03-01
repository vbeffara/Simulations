#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/Stream_lib.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub H("Streams and permutations", argc, argv, "n=10");
    for (int k = 1; k <= int(H['n']); ++k) {
        spdlog::info("n: {:>2}  permutations: {:>7}  partitions: {:>2}", k, size(permutations(k)), size(partitions(k)));
    }
}
