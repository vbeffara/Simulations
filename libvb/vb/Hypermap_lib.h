#pragma once
#include <vb/Hypermap.h>
#include <map>

namespace vb {
	Hypermap H_T ();         	// Triangle on the sphere
	Hypermap H_TT ();        	// Two triangles, this one is invalid but some of the algorithms work
	Hypermap H_TL ();        	// Triangular lattice
	Hypermap H_TL2 ();       	// Triangular lattice with one additional vertex of degree 2
	Hypermap H_TL3 ();       	// Triangular lattice with an eye
	Hypermap H_H1 ();        	// Planar hypermap, Zvonkin p.3 fig.4
	Hypermap H_H2 ();        	// This is a toric graph whose lift is the centered square lattice. Encode using images
	Hypermap H_H67 ();       	// This is H2 with one additional site
	Hypermap H_H3 ();        	// Planar map, Zvonkin p.4 fig.5
	Hypermap H_C5 ();        	// C5 graph
	Hypermap H_SV ();        	// SV graph
	Hypermap H_B ();         	// Bipartite version of H67
	Hypermap H_E ();         	// Tripartite of passport (8,8,62)
	Hypermap H_KS5 ();       	// Degree 5 example in Khadjavi-Scharaschkin, signature (23,23,5)
	Hypermap H_KS5T ();      	// Triangulation from KS5
	Hypermap H_artem (int n);	// Triangular lattice with one edge type split into n edges

	class HLib : public std::map <std::string,Hypermap> { public: HLib (); };
}
