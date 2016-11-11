#pragma once
#include <vb/Array.h>
#include <vb/Hypermap.h>
#include <vb/Stream.h>

namespace vb {
	Stream <std::vector<unsigned long>> partitions (unsigned long n, unsigned long m = 1);	// Partitions of n with piece size at least m
	Stream <std::vector<unsigned long>> tuples (unsigned long k, unsigned long n);        	// Ordered, distinct k-tuples in [0,n-1]
	Stream <std::vector<unsigned long>> cycles (unsigned long k, unsigned long n);        	// k-tuples up to cyclic permutation

	template <typename T> Stream<coo> coos (const Array<T> & A) { return Stream<coo> ([&A](Sink<coo> & yield) {
		for (long y=0; y<A.hh; ++y) for (long x=0; x<A.ww; ++x) yield(coo(x,y));
	});}

	Stream <Permutation> permutations (unsigned long n);
	Stream <Permutation> permutations (std::vector<unsigned long> s);

	Stream <Hypermap> hypermaps (const std::vector<unsigned long> & s, const std::vector<unsigned long> & a, const std::vector<unsigned long> & p);
}
