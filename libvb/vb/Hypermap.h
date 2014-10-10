#pragma once /// @file
#include <vb/Permutation.h>
#include <iostream>
#include <cassert>

namespace vb {
	class Hypermap {
	public:
		Hypermap (Permutation s, Permutation a, Permutation p)	: sigma(s), alpha(a), phi(p), initial(s.size(),true)       	{}
		Hypermap (Cycles s, Cycles a, Cycles p)               	: Hypermap (Permutation(s), Permutation(a), Permutation(p))	{}

		bool operator== (const Hypermap & o) const { return (sigma==o.sigma) && (alpha==o.alpha); }

		unsigned n_black () const { return sigma.cycles().size(); }
		unsigned n_white () const { return alpha.cycles().size(); }
		unsigned n_faces () const { return phi.cycles().size(); }
		unsigned n_edges () const { return sigma.size(); }

		bool	validate        	() const;
		bool	is_graph        	() const;
		bool	is_triangulation	() const;
		bool	is_simple       	() const; // ! parallel but non-consecutive edges (like an eye) are not detected

		int euler () const { return n_black() + n_white() - n_edges() + n_faces(); }
		int genus () const { return 1-euler()/2; }

		void output_dot      	(std::ostream & os);
		void output_graph_dot	(std::ostream & os);

		void flip (unsigned e);

		void	relabel	(const Permutation & p);

		void	normalize  	();
		void	mirror     	();
		void	dual       	();
		void	simplify2  	();
		void	simplify   	();
		void	split_edges	();

		Permutation sigma, alpha, phi;
		std::vector<bool> initial;

		std::string prog;

	private:
		Permutation	rebasing	(unsigned i)	const;
		Permutation	rebasing	()          	const;
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
