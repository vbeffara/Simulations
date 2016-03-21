#include <boost/coroutine/all.hpp>
#include <vb/Hub.h>
#include <vb/Permutation.h>
#include <vb/math.h>

using namespace std;
using namespace vb;

template <typename T> using coro = boost::coroutines::coroutine<T>;
template <typename T> using Stream = typename coro<T>::pull_type;
template <typename T> using Sink = typename coro<T>::push_type;

auto genperm (int n) {
    return Stream<Permutation> ([n](Sink<Permutation> & yield) {
        Permutation p(n);
        for (int i=0; i<fact(n); ++i) { yield(p); next_permutation(p.begin(),p.end()); }
    });
}

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    for (auto p : genperm(H['n'])) if (p[0]==2) cout << p << endl;
}
