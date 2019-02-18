#include <vb/Stream_lib.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub H("Streams and permutations", argc, argv, "n=10");
    for (int k = 1; k <= int(H['n']); ++k) {
        H.L->info("n: {:>2}  permutations: {:>7}  partitions: {:>2}", k, size(permutations(k)), size(partitions(k)));
    }
}
