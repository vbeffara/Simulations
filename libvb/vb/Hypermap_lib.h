#pragma once
#include <vb/Hypermap.h>

namespace vb {
	Hypermap H_T ();   	// Triangle on the sphere
	Hypermap H_TT ();  	// Two triangles, this one is invalid but some of the algorithms work.
	Hypermap H_H1 ();  	// This is the planar hypermap in Zvonkine's paper. Encode using cycles.
	Hypermap H_H2 ();  	// This is a toric graph whose lift is the centered square lattice. Encode using images.
	Hypermap H_H67 (); 	// This is H2 with one additional site.
	Hypermap H_H3 ();  	// The envelope-house in Zvonkine.
	Hypermap H_C5 ();  	// C5 graph
	Hypermap H_SV ();  	// SV graph
	Hypermap H_B ();   	// Bipartite version of H67
	Hypermap H_E ();   	// Tripartite of passport (8,8,62)
	Hypermap H_KS5 (); 	// Degree 5 example in Khadjavi-Scharaschkin, signature (23,23,5)
	Hypermap H_KS5T ();	// Triangulation from KS5
}
