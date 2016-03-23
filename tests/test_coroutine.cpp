#include <vb/Hub.h>
#include <vb/Permutation.h>
#include <vb/math.h>

using namespace std;
using namespace vb;

auto genperm (int n) {
    return Stream<Permutation> ([n](Sink<Permutation> & yield) {
        Permutation p(n);
        do { yield(p); } while ( next_permutation(p.begin(),p.end()) );
    });
}

bool good (const Permutation & p) { return p[0]==2; }
int sec (const Permutation & p) { return p[2]; }

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    auto S = genperm (H['n']);
    auto SS = filter (good,S);
    auto SSS = fmap<Permutation,int> (sec,SS);
    for (auto p : SSS) cout << p << endl;
}
