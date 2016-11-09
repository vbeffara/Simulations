#pragma once
#include <vb/Permutation.h>
#include <boost/coroutine2/all.hpp>

namespace vb {
	template <typename T> using Stream = boost::coroutines2::detail::pull_coroutine<T>;
	template <typename T> using Sink = boost::coroutines2::detail::push_coroutine<T>;

	template <typename T> int size (Stream <T> S) {
		int out=0; for (auto & i : S) { (void)i; out++; } return out;
	}

	template <typename T> auto take (int n, Stream <T> && S) {
		return Stream <T> ([n, S{std::move(S)}] (Sink <T> & yield) mutable {
			if (n<=0) return; int nn=n;
			for (auto x : S) { yield(x); --nn; if (nn==0) break; }
		});
	}

	template <typename F, typename T> auto filter (F && f, Stream <T> && S) {
		return Stream <T> ([&f, S{std::move(S)}] (Sink<T> & yield) mutable {
			for (auto x : S) if (f(x)) yield(x);
		});
	}

	template <typename F, typename T> auto fmap (F && f, Stream <T> && S) {
		return Stream <decltype(f(T()))> ([&f, S{std::move(S)}] (Sink<decltype(f(T()))> & yield) mutable {
			for (auto x : S) yield(f(x));
		});
	}

	template <typename T> auto take (int n, Stream <T> & S) { return take (n, std::move(S)); }
	template <typename F, typename T> auto filter (F && f, Stream <T> & S) { return filter (f, std::move(S)); }
	template <typename F, typename T> auto fmap (F && f, Stream <T> & S) { return fmap (f, std::move(S)); }

	Stream <int> ints ();

	Stream <std::vector<unsigned long>> partitions (unsigned long n, unsigned long m = 1);	// Partitions of n with piece size at least m
	Stream <std::vector<unsigned long>> tuples (unsigned long k, unsigned long n);        	// Ordered, distinct k-tuples in [0,n-1]
	Stream <std::vector<unsigned long>> cycles (unsigned long k, unsigned long n);        	// k-tuples up to cyclic permutation

	Stream <Permutation> permutations (unsigned long n);
	Stream <Permutation> permutations (std::vector<unsigned long> s);
}
