#pragma once /// @file
#include <vector>

namespace vb {
	using Cycles = std::vector<std::vector<int>>;

	class Permutation : public std::vector<int> {
	public:
		Permutation (int n = 0)         	: std::vector<int> (n)	{ for (int i=0; i<n; ++i) at(i)=i; }
		Permutation (std::vector<int> s)	: std::vector<int> (s)	{ }
		Permutation (Cycles & c);

		bool is_identity () const;

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $o \circ this$.

		Cycles cycles () const;

		std::vector<int>	l;
	};

	std::ostream & operator<< (std::ostream &os, const Permutation &P);
}
