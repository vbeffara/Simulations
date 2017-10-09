#pragma once
#include <vb/Array.h>
#include <vb/Hypermap.h>
#include <vb/Stream.h>

namespace vb {
	Stream <std::vector<int>> partitions (int n, int m = 1);	// Partitions of n with piece size at least m
	Stream <std::vector<int>> tuples     (int k, int n);       	// Ordered, distinct k-tuples in [0,n-1]
	Stream <std::vector<int>> cycles     (int k, int n);       	// k-tuples up to cyclic permutation

	template <typename T> Stream<coo> coos (const Array<T> & A) { return Stream<coo> ([&A](Sink<coo> & yield) {
		for (long y=0; y<A.hh; ++y) for (long x=0; x<A.ww; ++x) yield(coo(x,y));
	});}

	Stream <Permutation> permutations (int n);
	Stream <Permutation> permutations (std::vector<int> s);

	Stream <Hypermap> hypermaps (const std::vector<int> & s, const std::vector<int> & a, const std::vector<int> & p);
}
