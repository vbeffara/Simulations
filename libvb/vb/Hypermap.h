#pragma once /// @file
#include <vb/Permutation.h>
#include <vb/cpx.h>
#ifdef None
#undef None
#endif
#include <yaml-cpp/yaml.h>

namespace vb {
	class Hypermap {
	public:
		struct Vertex	{ unsigned i=0;	cpx z;         	double r=1.0;	unsigned bone=0;	std::vector<unsigned> adj;	bool fixed = false;	};
		struct Edge  	{ unsigned i=0;	unsigned src=0;	double a=NAN;	                	                          	                   	};

		Hypermap (Permutation s, Permutation a, Permutation p)	: sigma(std::move(s)), alpha(std::move(a)), phi(std::move(p)), initial(sigma.size(),3)	{}
		Hypermap (Cycles s, Cycles a, Cycles p)               	: Hypermap (Permutation(s), Permutation(a), Permutation(p))                           	{}
		Hypermap () = default;

		void from_hypermap	();	//< If the Hypermap part is correct, fill in V and E.

		bool operator== (const Hypermap & o) const { return (sigma==o.sigma) && (alpha==o.alpha); }

		bool    	validate        	() const;
		bool    	is_graph        	() const;
		bool    	is_triangulation	() const;
		bool    	is_simple       	(unsigned d = 2) const; // ! parallel but non-consecutive edges (like an eye) are not detected
		int     	euler           	() const;
		unsigned	genus           	() const;

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

		double alpha_xyz	(double x, double y, double z)	const;
		double ccn      	(int n)                       	const;

		int mode = 228; std::string title;

	private:
		Permutation	rebasing	(unsigned i)	const;
		Permutation	rebasing	()          	const;
	};

	std::ostream & operator<< (std::ostream &os, Hypermap &H);
}

namespace YAML {
	template<> struct convert <vb::Hypermap> {
		static Node encode (const vb::Hypermap & h);
		static bool decode (const Node & node, vb::Hypermap & h);
	};
}
