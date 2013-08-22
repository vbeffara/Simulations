#include <vb/CL_Parser.h>
#include <vb/Hypermap.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <cassert>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {

	// Triangle on the sphere
	Hypermap T;
	T.sigma	= {{0,1},{2,3},{4,5}};
	T.alpha	= {{0,2},{3,4},{1,5}};
	T.phi  	= {{0,3,5},{1,4,2}};

	// Two triangles, this one is invalid but some of the algorithms work.
	Hypermap TT;
	TT.sigma	= {{0,1},{2,3},{4,5}, {6,7},{8,9},{10,11}};
	TT.alpha	= {{0,2},{3,4},{1,5}, {6,8},{9,10},{7,11}};
	TT.phi  	= {{0,3,5},{1,4,2},   {6,9,11}, {7,10,8}};

	// This is the planar hypermap in Zvonkine's paper. Encode using cycles.
	Hypermap H1;
	H1.sigma	= { { 0, 1, 2 }, { 3, 4 }, { 5 }, { 6, 7, 8 } };
	H1.alpha	= { { 0, 3 }, { 1, 8, 2 }, { 4, 5, 6 }, { 7 } };
	H1.phi  	= { { 0, 4, 8 }, { 1 }, { 2, 7, 6, 5, 3 } };

	// This is a toric graph whose lift is the centered square lattice. Encode using images.
	Hypermap H2;
	H2.sigma	= { 1, 2, 3, 4, 5, 6, 7, 0, 11, 8, 9, 10 };
	H2.alpha	= { 4, 11, 6, 10, 0, 9, 2, 8, 7, 5, 3, 1 };
	H2.phi  	= { 3, 8, 5, 11, 7, 10, 1, 9, 6, 4, 2, 0 };

	// The envelope-house in Zvonkine.
	Hypermap H3;
	H3.sigma	= {{0,1,2},{3,4},{5,6,7,8},{9,10},{11,12,13}};
	H3.alpha	= {{0,13},{1,6},{2,3},{4,5},{7,12},{8,9},{10,11}};
	H3.phi  	= {{0,12,6},{1,5,3},{2,4,8,10,13},{7,11,9}};

	// Hyperbolic surface of genus 2 from the octogon.
	Hypermap O;
	O.sigma	= {3,2,4,1,7,6,0,5};
	O.alpha	= {{0,1},{2,3},{4,5},{6,7}};
	O.phi  	= {3,6,0,1,7,2,4,5};

	// Experiments

	CL_Parser CLP (argc,argv,"n=5,t=1000");
	Hypermap H=T;

	for (int i=0; i<int(CLP('n')); ++i) H = H.split_edges();

	assert(H.validate()); cerr << H;
	assert (H.is_triangulation());

	{	int T=CLP('t'); ProgressBar PB (T);
	 	for (int t=0; t<T; ++t) {
	 		H.flip(prng.uniform_int(H.n_edges()),true);
	 		if (t%1000 == 0) PB.set(t);
	 	}
	}

	H.sigma.use_s(); H.phi.use_s();

	H.output_graph_dot(cout);

	return 0;
}
