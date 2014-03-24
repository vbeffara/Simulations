#pragma once /// @file
#include <vb/Permutation.h>
#include <iostream>
#include <cassert>

namespace vb {
	class Hypermap {
	public:
		Hypermap (Cycles s, Cycles a, Cycles p) : Hypermap (Permutation(s), Permutation(a), Permutation(p)) {}
		Hypermap (Permutation s, Permutation a, Permutation p) : sigma(s), alpha(a), phi(p) {}

		int n_black () const { return sigma.cycles().size(); }
		int n_white () const { return alpha.cycles().size(); }
		int n_faces () const { return phi.cycles().size(); }
		int n_edges () const { return sigma.size(); }

		bool validate ();
		bool is_graph () { for (auto v : alpha.cycles()) if (v.size() != 2) return false; return true; }
		bool is_triangulation () {
			if (!(is_graph())) return false;
			for (auto f : phi.cycles()) if (f.size() != 3) return false;
			return true;
		}

		int euler () { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () { return 1-euler()/2; }

		void output_dot      	(std::ostream & os);
		void output_graph_dot	(std::ostream & os);

		Hypermap split_edges (); ///< Turn white vertices into black and then complete into a new hypermap.

		void flip (int e);

		Permutation sigma, alpha, phi; // black, white, faces
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
