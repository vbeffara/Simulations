#pragma once /// @file
#include <vector>

namespace vb {
	using Cycles = std::vector<std::vector<int>>;

	class Permutation : public std::vector<int> {
	public:
		Permutation (std::vector<int> s = {})	: std::vector<int> (s)	{ use_s(); }
		Permutation (Cycles cc)              	: c(cc)               	{ use_c(); }

		Permutation & operator= (const std::vector<int> s)              	{ vector<int>::operator=(s);	use_s(); return *this; }
		Permutation & operator= (const std::vector<std::vector<int>> cc)	{ c = cc;                   	use_c(); return *this; }

		void use_s ();
		void use_c ();

		bool is_identity ();

		Permutation inverse  	()                     	const;
		Permutation operator*	(const Permutation & o)	const; // this then o i.e. $o \circ this$.

		std::vector<std::vector<int>> & cycles () { return c; }

		std::vector<int>	l;

	private:
		std::vector<std::vector<int>>	c;
	};


	std::ostream & operator<< (std::ostream &os, Permutation &P);
}
