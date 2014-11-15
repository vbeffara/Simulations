#pragma once /// @file
#include <vb/Permutation.h>
#include <vb/cpx.h>

namespace vb {
	class Hypermap {
	public:
		struct Vertex	{ unsigned i;	cpx z;       	double r=1.0;	unsigned bone;	std::vector<unsigned> adj;	bool fixed = false;	};
		struct Edge  	{ unsigned i;	unsigned src;	double a=NAN;	              	                          	                   	};

		Hypermap (Permutation s, Permutation a, Permutation p)	: sigma(s), alpha(a), phi(p), initial(s.size(),3)          	{}
		Hypermap (Cycles s, Cycles a, Cycles p)               	: Hypermap (Permutation(s), Permutation(a), Permutation(p))	{}
		Hypermap () {}

		void from_hypermap	();	//< If the Hypermap part is correct, fill in V and E.

		bool operator== (const Hypermap & o) const { return (sigma==o.sigma) && (alpha==o.alpha); }

		bool    	validate        	() const;
		bool    	is_graph        	() const;
		bool    	is_triangulation	() const;
		bool    	is_simple       	(unsigned d = 2) const; // ! parallel but non-consecutive edges (like an eye) are not detected
		int     	euler           	() const;
		unsigned	genus           	() const;

		std::string name () const; // For triangulations, assumed to be normalized

		void flip (unsigned e);

		void	acpa	();

		void	relabel	(const Permutation & p);

		void	normalize  	();
		void	mirror     	();
		void	dual       	();
		void	simplify1  	();
		void	simplify2  	();
		void	simplify   	(int d = 2);
		void	split_edges	();
		void	dessin     	();

		Permutation sigma, alpha, phi;

		std::vector<unsigned> initial;
		std::vector<Vertex>	V;
		std::vector<Edge>  	E;

		std::string prog;

	protected:
		double alpha_xyz	(double x, double y, double z)	const;
		double ccn      	(int n)                       	const;

		unsigned mode; std::string title;

	private:
		Permutation	rebasing	(unsigned i)	const;
		Permutation	rebasing	()          	const;
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}
