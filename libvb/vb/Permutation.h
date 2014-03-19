#pragma once /// @file
#include <vector>

namespace vb {
	using Cycles = std::vector<std::vector<int>>;

	class Permutation : public std::vector<int> {
	public:
		Permutation (int n = 0)         	: std::vector<int> (n)	{ for (int i=0; i<n; ++i) at(i)=i; use_s(); }
		Permutation (std::vector<int> s)	: std::vector<int> (s)	{ use_s(); }
		Permutation (Cycles cc)         	: c(cc)               	{ use_c(); }

		void use_s ();
		void use_c ();

		bool is_identity ();

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $o \circ this$.

		Cycles & cycles () { return c; }

		std::vector<int>	l;

	private:
		Cycles	c;
	};


	std::ostream & operator<< (std::ostream &os, Permutation &P);
}
