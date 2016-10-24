#pragma once
#include <boost/coroutine2/all.hpp>
#include <vector>

namespace vb {
	template <typename T> using Stream = boost::coroutines2::detail::pull_coroutine<T>;
	template <typename T> using Sink = boost::coroutines2::detail::push_coroutine<T>;

	template <typename T> int size (Stream <T> S) {
		int out=0; for (auto & i : S) { (void)i; out++; } return out;
	}

	template <typename T> auto take (int n, Stream <T> & S) {
		return Stream <T> ([n,&S] (Sink <T> & yield) {
			if (n<=0) return; int nn=n;
			for (auto x : S) { yield(x); --nn; if (nn==0) break; }
		});
	}

	template <typename F, typename T> auto filter (F && f, Stream <T> & S) {
		return Stream <T> ([&](Sink<T> & yield) { for (auto x : S) if (f(x)) yield(x); });
	}

	template <typename F, typename U> auto fmap (F && f, Stream <U> & S) {
		return Stream <decltype(f(U()))> ([&](Sink<decltype(f(U()))> & yield) { for (auto x : S) yield(f(x)); });
	}

	Stream <std::vector<unsigned long>> partitions (unsigned long n, unsigned long m = 1);	// Partitions of n with piece size at least m
	Stream <std::vector<unsigned long>> tuples (unsigned long k, unsigned long n);        	// Ordered, distinct k-tuples in [0,n-1]
	Stream <std::vector<unsigned long>> cycles (unsigned long k, unsigned long n);        	// k-tuples up to cyclic permutation
}
