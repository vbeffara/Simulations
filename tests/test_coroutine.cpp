#include <boost/coroutine/all.hpp>
#include <vb/Hub.h>
#include <vb/Permutation.h>
#include <vb/math.h>

using namespace std;
using namespace vb;

template <typename T> using coro = boost::coroutines::coroutine<T>;

auto genperm (int n) {
    return coro<Permutation>::pull_type ([n](coro<Permutation>::push_type & yield) {
        Permutation p(n);
        for (int i=0; i<fact(n); ++i) { yield(p); next_permutation(p.begin(),p.end()); }
    });
}

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    for (auto p : genperm(H['n'])) cout << p << endl;
}
