#include <vb/Hub.h>
// #include <vb/Permutation.h>
#include <boost/coroutine2/all.hpp>

using namespace vb; using namespace std;

template <typename T> using coro_t = boost::coroutines2::coroutine<T>;
template <typename T> using Stream2 = typename coro_t<T>::pull_type;
template <typename T> using Sink2 = typename coro_t<T>::push_type;

// auto genperm2 (int n) {
//     return Stream2 <Permutation> ([n](Sink2 <Permutation> & yield) {
//         Permutation p(n);
//         do { yield(p); } while ( next_permutation(p.begin(),p.end()) );
//     });
// }

auto ints () {
    return Stream2 <int> ([](Sink2 <int> & yield) {
		for (int i=0 ;; ++i) yield(i);
    });
}

template <typename T> auto take (int n, Stream2 <T> & S) {
	return Stream2 <T> ([n,&S] (Sink2 <T> & yield) {
		int i=0;
		for (auto x : S) { if (i>=n) break; yield(x); ++i; }
	});
}

// bool good (const Permutation & p) { return p[0]==2; }
// int sec (const Permutation & p) { return p[2]; }

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=10");
    auto S = ints ();
    auto SS = take <int> (H['n'], S);
    for (auto p : SS) H.L->info (p);
    // auto SS = filter (good,S);
    // auto SSS = fmap<Permutation,int> (sec,SS);
}
