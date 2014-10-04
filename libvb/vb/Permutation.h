#pragma once /// @file
#include <vector>
#include <iostream>

namespace vb {
	using Cycles = std::vector<std::vector<unsigned>>;

	class Permutation : public std::vector<unsigned> {
	public:
		Permutation (unsigned n = 0)         	: std::vector<unsigned> (n)	{ for (unsigned i=0; i<n; ++i) at(i)=i; }
		Permutation (std::vector<unsigned> s)	: std::vector<unsigned> (s)	{ }
		Permutation (Cycles & c);

		bool is_identity () const;

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $this * o = o \circ this$.

		Cycles               	cycles ()   	const;
		std::vector<unsigned>	signature ()	const;

		std::vector<unsigned>	l;
	};

	std::ostream & operator<< (std::ostream &os, const Permutation &P);
}
