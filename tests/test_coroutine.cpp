#include <vb/Hub.h>
#include <vb/Stream.h>
#include <vb/math.h>

using namespace std;
using namespace vb;

bool good (const Permutation & p) { return p[0]==2; }
int sec (const Permutation & p) { return p[2]; }

template <typename F, typename U> auto fmap2 (F && f, Stream <U> && S) {
	return Stream <decltype(f(U()))> ([&f, S{move(S)}](Sink<decltype(f(U()))> & yield) mutable {
		for (auto x : S) yield(f(x));
	});
}

template <typename F, typename U> auto fmap2 (F && f, Stream <U> & S) {
	return fmap2 (f, move(S));
}

int main(int argc, char ** argv) {
    H.init ("Testing coroutines", argc, argv, "n=5");
    auto S = permutations (int(H['n']));
    auto SS = filter (good,S);
    auto SSS = fmap (sec, SS);
    auto SSSS = take (8,SSS);
    for (auto p : SSSS) H.L->info ("{}",p);
}
