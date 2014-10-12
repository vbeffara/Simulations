#pragma once

// Toroidal: Hypermap-based triangulation of genus 1
//
// Mode: 1 -> disks along the bones
//       2 -> disks elsewhere
//       4 -> edges along the bones
//       8 -> edges around the bones
//      16 -> edges elsewhere
//      32 -> big points at vertices (initial[i]&2)
//      64 -> circles at midpoints (initial[i]&4)
//     128 -> red circles at faces (initial[i]&8)
//     256 -> stars at faces (variation on 128)
//
// Good modes are:
//    3 -> circle packing;
//    4 -> just the bones;
//   17 -> chain of empty disks plus small triangles;
//   24 -> whole triangulation;
//  228 -> dessin (bones, vertices in black, midpoints in white, centers of faces)

#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/cpx.h>
#include <vector>

namespace vb {
	class Toroidal : public Hypermap {
	public:
		class Vertex	{ public: cpx z;  	double r;	unsigned bone; std::vector<int> adj; std::vector<unsigned> edges;	Vertex () { r=1; } };
		class Edge  	{ public: int src;	double a; };

		Toroidal (Hypermap M, Hub H_);

		void split_edges  	();	//< Add one vertex at the midpoint of each edge.
		void from_hypermap	();	//< If the Hypermap part is correct, fill in V and E.
		void acpa         	();	//< Compute the radii of the circle packing.
		void pack         	();	//< Run acpa() and then compute an embedding into a torus.
		void flip         	();	//< Map everything by z -> -z.
		void output_pdf   	();	//< Make a PDF of a few periods of the covering.

		cpx m;

	private:
		std::vector<Vertex>	V;
		std::vector<Edge>  	E;

		int n_skel;
		Hub H;
	};
}