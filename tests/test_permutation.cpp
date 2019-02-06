#include <algorithm>
#include <iomanip>
#include <vb/Stream_lib.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    H.init("Streams and permutations", argc, argv, "n=10");
    for (int k = 1; k <= int(H['n']); ++k) {
        cout << "n: " << setw(2) << k;
        cout << "\tpermutations: " << setw(7) << size(permutations(k));
        cout << "\tpartitions: " << setw(2) << size(partitions(k)) << endl;
    }
}
