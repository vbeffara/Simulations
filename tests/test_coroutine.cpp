#include <boost/coroutine/all.hpp>
#include <vb/Hub.h>
#include <vb/Permutation.h>
#include <vb/math.h>

using namespace std;
using namespace vb;

template <typename T> using Stream = boost::coroutines::pull_coroutine<T>;
template <typename T> using Sink = boost::coroutines::push_coroutine<T>;

auto genperm (int n) {
    return Stream<Permutation> ([n](Sink<Permutation> & yield) {
        Permutation p(n);
        for (int i=0; i<fact(n); ++i) { yield(p); next_permutation(p.begin(),p.end()); }
    });
}

bool good (const Permutation & p) { return p[0]==2; }
int sec (const Permutation & p) { return p[2]; }

template <typename T> auto filter (auto && f, Stream<T> & S) {
	return Stream<T> ([&](Sink<T> & yield) { for (auto x : S) if (f(x)) yield(x); });
}

template <typename U, typename V> auto fmap (auto f, Stream<U> & S) {
	return Stream<V> ([&](Sink<V> & yield) { for (auto x : S) yield(f(x)); });
}

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    auto S = genperm (H['n']);
    auto SS = filter (good,S);
    auto SSS = fmap<Permutation,int> (sec,SS);
    for (auto p : SSS) cout << p << endl;
}
