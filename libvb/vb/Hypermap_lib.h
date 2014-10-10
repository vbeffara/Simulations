#pragma once
#include <vb/Hypermap.h>

namespace vb {
	Hypermap H_T ();         	// Triangle on the sphere
	Hypermap H_TT ();        	// Two triangles, this one is invalid but some of the algorithms work
	Hypermap H_TL ();        	// Triangular lattice
	Hypermap H_TL2 ();       	// Triangular lattice with one additional vertex of degree 2
	Hypermap H_TL3 ();       	// Triangular lattice with an eye
	Hypermap H_H1 ();        	// This is the planar hypermap in Zvonkine's paper. Encode using cycles.
	Hypermap H_H2 ();        	// This is a toric graph whose lift is the centered square lattice. Encode using images
	Hypermap H_H67 ();       	// This is H2 with one additional site
	Hypermap H_H3 ();        	// The envelope-house in Zvonkine
	Hypermap H_C5 ();        	// C5 graph
	Hypermap H_SV ();        	// SV graph
	Hypermap H_B ();         	// Bipartite version of H67
	Hypermap H_E ();         	// Tripartite of passport (8,8,62)
	Hypermap H_KS5 ();       	// Degree 5 example in Khadjavi-Scharaschkin, signature (23,23,5)
	Hypermap H_KS5T ();      	// Triangulation from KS5
	Hypermap H_artem (int n);	// Triangular lattice with one edge type split into n edges

	Hypermap H_lib (std::string s) {
		if (s == "T")   	return H_T   	();
		if (s == "TT")  	return H_TT  	();
		if (s == "TL")  	return H_TL  	();
		if (s == "TL2") 	return H_TL2 	();
		if (s == "TL3") 	return H_TL3 	();
		if (s == "H1")  	return H_H1  	();
		if (s == "H2")  	return H_H2  	();
		if (s == "H67") 	return H_H67 	();
		if (s == "H3")  	return H_H3  	();
		if (s == "C5")  	return H_C5  	();
		if (s == "SV")  	return H_SV  	();
		if (s == "B")   	return H_B   	();
		if (s == "E")   	return H_E   	();
		if (s == "KS5") 	return H_KS5T	();
		if (s == "KS5T")	return H_KS5T	();
		throw "err";
	}
}
