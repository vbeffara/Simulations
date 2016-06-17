#pragma once
#include <boost/coroutine/all.hpp>
#include <vector>

namespace vb {
	template <typename T> using Stream = boost::coroutines::pull_coroutine<T>;
	template <typename T> using Sink = boost::coroutines::push_coroutine<T>;

	template <typename T> int size (Stream<T> S) {
		int out=0; for (auto & i : S) { (void)i; out++; } return out;
	}

	template <typename T, typename F> auto filter (F && f, Stream<T> & S) {
		return Stream<T> ([&](Sink<T> & yield) { for (auto x : S) if (f(x)) yield(x); });
	}

	template <typename U, typename V, typename F> auto fmap (F && f, Stream<U> & S) {
		return Stream<V> ([&](Sink<V> & yield) { for (auto x : S) yield(f(x)); });
	}

	Stream <std::vector<unsigned>> partitions (unsigned n, unsigned m = 1);	// Partitions of n with piece size at least m
	Stream <std::vector<unsigned>> tuples (unsigned k, unsigned n);        	// Ordered, distinct k-tuples in [0,n-1]
	Stream <std::vector<unsigned>> cycles (int k, int n);                  	// k-tuples up to cyclic permutation
}
