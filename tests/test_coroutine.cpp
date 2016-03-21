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

bool good (const Permutation & p) { return p[0]==2; }

template <typename T> auto filter (Stream<T> & S, auto && f) {
	return Stream<T> ([&](Sink<T> & yield) { for (auto x : S) if (f(x)) yield(x); });
}

template <typename T> auto filter (auto && f) {
	return [&](Stream<T> & S) { return filter<T> (S,f); };
}

template <typename U, typename V> auto fmap (Stream<U> & S, auto && f) {
	return Stream<V> ([&](Sink<V> & yield) { for (auto x : S) yield(f(x)); });
}

template <typename U, typename V> auto fmap (auto && f) {
	return [&](Stream<U> & S) { return fmap<U,V> (S,f); };
}

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    Stream<Permutation> S = genperm(H['n']);
    Stream<Permutation> SS = filter<Permutation> (good) (S);
    Stream<int> SSS = fmap<Permutation,int> ([](Permutation p) { return p[2]; }) (SS);
    for (auto p : SSS) cout << p << endl;
}
