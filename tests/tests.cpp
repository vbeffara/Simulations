#define BOOST_TEST_MODULE libvb unit tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vb/Array.h>
#include <vb/TriMatrix.h>

using namespace vb;

// Data structures

BOOST_AUTO_TEST_CASE (test_Array) {
	Array<int> A (23,45,1);
	BOOST_TEST (A.contains({6,3}));
	BOOST_TEST (!A.contains({23,1}));
	BOOST_TEST (!A.contains({5,45}));

	A.put({1,1}, 2);
	BOOST_TEST (A.at({3,4}) == 1);
	BOOST_TEST (A.at({1,1}) == 2);

	A.putp({5*23+4,7*45+10}, 0);
	BOOST_TEST (A.atp({23+4,-45+10}) == 0);

	A.at(A.rand()) += 10;
	int s = 0; for (auto i : A) s += i;
	BOOST_TEST (s == 23*45 + 10);
}

BOOST_AUTO_TEST_CASE (test_TriMatrix) {
	TriMatrix <int> M;
	M.put({35,42}, 3);
	M.put({1234,5678}, 5);
	M.put({91823749,-2793474}, 23);

	BOOST_TEST (M.at({35,42}) == 3);
	BOOST_TEST (M.at({1234,5678}) == 5);
	BOOST_TEST (M.at({91823749,-2793474}) == 23);
	BOOST_TEST (M.at({3,4}) == 0);
	BOOST_TEST (M.at({981327,2371827}) == 0);
}

// Below is still to be done

BOOST_AUTO_TEST_CASE (test_Auto) {}

BOOST_AUTO_TEST_CASE (test_Automaton) {}

BOOST_AUTO_TEST_CASE (test_AutoWindow) {}

BOOST_AUTO_TEST_CASE (test_Bitmap) {}

BOOST_AUTO_TEST_CASE (test_CoarseImage) {}

BOOST_AUTO_TEST_CASE (test_Color) {}

BOOST_AUTO_TEST_CASE (test_Coloring) {}

BOOST_AUTO_TEST_CASE (test_Console) {}

BOOST_AUTO_TEST_CASE (test_Constellation) {}

BOOST_AUTO_TEST_CASE (test_Constellation0) {}

BOOST_AUTO_TEST_CASE (test_Constellation1) {}

BOOST_AUTO_TEST_CASE (test_coo) {}

BOOST_AUTO_TEST_CASE (test_cpx) {}

BOOST_AUTO_TEST_CASE (test_Cube) {}

BOOST_AUTO_TEST_CASE (test_Elliptic) {}

BOOST_AUTO_TEST_CASE (test_Figure) {}

BOOST_AUTO_TEST_CASE (test_Hub) {}

BOOST_AUTO_TEST_CASE (test_Hypermap) {}

BOOST_AUTO_TEST_CASE (test_Hypermap_lib) {}

BOOST_AUTO_TEST_CASE (test_Image) {}

BOOST_AUTO_TEST_CASE (test_Lattice) {}

BOOST_AUTO_TEST_CASE (test_LinearAlgebra) {}

BOOST_AUTO_TEST_CASE (test_Map) {}

BOOST_AUTO_TEST_CASE (test_Minimizer) {}

BOOST_AUTO_TEST_CASE (test_NumberTheory) {}

BOOST_AUTO_TEST_CASE (test_Pairings) {}

BOOST_AUTO_TEST_CASE (test_Path) {}

BOOST_AUTO_TEST_CASE (test_Permutation) {}

BOOST_AUTO_TEST_CASE (test_Picture) {}

BOOST_AUTO_TEST_CASE (test_Point) {}

BOOST_AUTO_TEST_CASE (test_Polynomial) {}

BOOST_AUTO_TEST_CASE (test_Pov) {}

BOOST_AUTO_TEST_CASE (test_PRNG) {}

BOOST_AUTO_TEST_CASE (test_ProgressBar) {}

BOOST_AUTO_TEST_CASE (test_QuadTree) {}

BOOST_AUTO_TEST_CASE (test_RationalFraction) {}

BOOST_AUTO_TEST_CASE (test_Shape) {}

BOOST_AUTO_TEST_CASE (test_Spheroidal) {}

BOOST_AUTO_TEST_CASE (test_Toroidal) {}
