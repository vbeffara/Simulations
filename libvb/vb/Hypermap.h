#pragma once /// @file
#include <vector>
#include <cassert>
#include <iostream>

// Implementation of hypermaps as triples of permutations, following A. Zvonkin, "Belyi Functions: Examples, Properties, and Applications".

namespace vb {
	class Permutation : public std::vector<int> {
	public:
		Permutation (std::vector<int> s = {})         	: std::vector<int> (s)	{ s_to_c(); }
		Permutation (std::vector<std::vector<int>> cc)	: c(cc)               	{ c_to_s(); }

		Permutation & operator= (const std::vector<int> s)              	{ vector<int>::operator=(s);	s_to_c(); return *this; }
		Permutation & operator= (const std::vector<std::vector<int>> cc)	{ c = cc;                   	c_to_s(); return *this; }

		void s_to_c ();
		void c_to_s ();

		std::vector<std::vector<int>> c;
	};

	class Hypermap {
	public:
		void validate ();

		int n_black () { return sigma.c.size(); }
		int n_white () { return alpha.c.size(); }
		int n_faces () { return phi.c.size(); }
		int n_edges () { return sigma.size(); }

		bool is_graph () { for (std::vector<int> v : alpha.c) if (v.size() != 2) return false; return true; }

		int euler () { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () { return 1-euler()/2; }

		void output_dot (std::ostream & os) {
			std::vector<int> black(n_edges()), white(n_edges());
			for (int i=0; i<n_black(); ++i) for (int j : sigma.c[i]) black[j]=i;
			for (int i=0; i<n_white(); ++i) for (int j : alpha.c[i]) white[j]=i;
			os << "graph {" << std::endl;
			for (int i=0; i<n_black(); ++i) os << "  b" << i << " [fillcolor=\"grey\"];" << std::endl;
			for (int i=0; i<n_edges(); ++i) os << "  b" << black[i] << " -- w" << white[i] << ";" << std::endl;
			os << "}" << std::endl;
		}

		Permutation sigma, alpha, phi; // black, white, faces
	};

	std::ostream & operator<< (std::ostream &os, Permutation &P);
	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
