#include <vb/Hub.h>
#include <vb/Permutation.h>
#include <boost/coroutine2/all.hpp>

using namespace vb; using namespace std;

template <typename T> using coro_t = boost::coroutines2::coroutine<T>;
template <typename T> using Sink2 = typename coro_t<T>::push_type;

template <typename T> struct Stream2 : public coro_t<T>::pull_type {
    using coro_t<T>::pull_type::pull_type;
};

template <typename T, typename F> auto filter (F && f, Stream2 <T> & S) {
    return Stream2 <T> ([&](Sink2 <T> & yield) { for (auto x : S) if (f(x)) yield(x); });
}

auto genperm2 (int n) {
    return Stream2 <Permutation> ([n](Sink2 <Permutation> & yield) {
        Permutation p(n);
        do { yield(Permutation(p)); } while ( next_permutation(p.begin(),p.end()) );
    });
}

template <typename U, typename F> auto fmap (F && f, Stream2 <U> & S) {
    return Stream2 <decltype(f(U()))> ([&](Sink2 <decltype(f(U()))> & yield) { for (auto x : S) yield(f(x)); });
}

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

bool good (const Permutation & p) { return p[0]==2; }
int sec (const Permutation & p) { return p[2]; }

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    auto S = genperm2(H['n']);
    auto SS = filter (good,S);
    auto SSS = fmap (sec,SS);
    for (const auto & p : SSS) H.L->info ("{}",p);
}
