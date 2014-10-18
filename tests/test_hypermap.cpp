#include <vb/Constellation.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=3,m=228,d");
	// Hypermap M { {{1},{3},{5},{0,4,2}}, {{0,1},{2,3},{4,5}}, {{0,1,2,3,4,5}} };                                	// Graph tripod
	// Hypermap M { {{0,1,2}}, {{0},{1},{2}}, {{2,1,0}} };                                                        	// Dessin tripod
	// Hypermap M { {{0,1,2,3}}, {{0},{1},{2},{3}}, {{3,2,1,0}} };                                                	// Dessin 4-star
	// Hypermap M { {{0,1,2,3,4}}, {{0},{1},{2},{3},{4}}, {{4,3,2,1,0}} };                                        	// Dessin 5-star
	// Hypermap M { {{1},{5},{7},{3,4},{2,6,0}}, {{2,1},{0,3},{4,5},{6,7}}, {{2,1,0,4,5,3,6,7}} };                	// Long graph tripod - take n=4
	// Hypermap M { {{2,1,0},{3}}, {{2},{1},{0,3}}, {{2,0,3,1}} };                                                	// Long dessin tripod - Lando page 87
	// Hypermap M { {{0,1,2},{3,4},{5,6}}, {{0},{1,3},{2,5},{4},{6}}, {{0,2,6,5,1,4,3}} };                        	// Long-legged tripod - Lando page 88
	// Hypermap M { {{4,1,2},{3,0},{5,6}}, {{4,3},{1},{2},{0,5},{6}}, {{4,0,6,5,3,2,1}} };                        	// Long-tailed tripod - Lando page 89 - FAIL?
	// Hypermap M { {{1},{5},{7},{9},{2,8,0},{4,3,6}}, {{2,1},{0,3},{4,5},{6,7},{8,9}}, {{2,1,0,4,5,6,7,3,8,9}} };	// Graph H - FAIL?
	// Hypermap M { {{0,1,2},{3},{4}}, {{0,3,4},{1},{2}}, {{0,4,3,2,1}} };                                        	// Dessin H
	// Hypermap M { {{0,2,3},{1}}, {{0,1},{2,3}}, {{0,1,3},{2}} };                                                	// Simple map - Lando page 107
	Hypermap M = H_H3();

	cout << M << endl;

	Constellation C (M,H);

	C.find(); C.belyi(); cout << endl << C;
}
