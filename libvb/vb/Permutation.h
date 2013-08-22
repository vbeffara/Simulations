#pragma once /// @file
#include <vector>
#include <iostream>

// Implementation of hypermaps as triples of permutations, following A. Zvonkin, "Belyi Functions: Examples, Properties, and Applications".

namespace vb {
	class Permutation : public std::vector<int> {
	public:
		Permutation (std::vector<int> s = {})         	: std::vector<int> (s)	{ use_s(); }
		Permutation (std::vector<std::vector<int>> cc)	: c(cc)               	{ use_c(); }

		Permutation & operator= (const std::vector<int> s)              	{ vector<int>::operator=(s);	use_s(); return *this; }
		Permutation & operator= (const std::vector<std::vector<int>> cc)	{ c = cc;                   	use_c(); return *this; }

		void use_s ();
		void use_c ();
		void labels () {
			l.resize(size());
			for (int i=0; i<c.size(); ++i) for (int j : c[i]) l[j]=i;
		}

		std::vector<std::vector<int>>	c;
		std::vector<int>             	l;
	};

	std::ostream & operator<< (std::ostream &os, Permutation &P);
}
