#pragma once /// @file
#include <vector>
#include <iostream>

// Implementation of hypermaps as triples of permutations, following A. Zvonkin, "Belyi Functions: Examples, Properties, and Applications".

namespace vb {
	class Permutation : public std::vector<int> {
	public:
		Permutation (std::vector<int> s = {})         	: std::vector<int> (s)	{ s_to_c(); labels(); }
		Permutation (std::vector<std::vector<int>> cc)	: c(cc)               	{ c_to_s(); labels(); }

		Permutation & operator= (const std::vector<int> s)              	{ vector<int>::operator=(s);	s_to_c(); return *this; }
		Permutation & operator= (const std::vector<std::vector<int>> cc)	{ c = cc;                   	c_to_s(); return *this; }

		void s_to_c ();
		void c_to_s ();
		void labels () {
			l.resize(size());
			for (int i=0; i<c.size(); ++i) for (int j : c[i]) l[j]=i;
		}

		std::vector<std::vector<int>>	c;
		std::vector<int>             	l;
	};

	std::ostream & operator<< (std::ostream &os, Permutation &P);
}
