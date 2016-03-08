#define BOOST_TEST_MODULE libvb unit tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vb/Array.h>
#include <vb/Color.h>
#include <vb/Hub.h>
#include <vb/NumberTheory.h>
#include <vb/ProgressBar.h>
#include <vb/TriMatrix.h>

using namespace vb; using namespace std;

// Data structures

BOOST_AUTO_TEST_CASE (test_Array) {
	Array<int> A (23,45,1);
	BOOST_CHECK (A.contains({6,3}));
	BOOST_CHECK (!A.contains({23,1}));
	BOOST_CHECK (!A.contains({5,45}));

	A.put({1,1}, 2);
	BOOST_CHECK (A.at({3,4}) == 1);
	BOOST_CHECK (A.at({1,1}) == 2);

	A.putp({5*23+4,7*45+10}, 0);
	BOOST_CHECK (A.atp({23+4,-45+10}) == 0);

	A.at(A.rand()) += 10;
	int s = 0; for (auto i : A) s += i;
	BOOST_CHECK (s == 23*45 + 10);
}

BOOST_AUTO_TEST_CASE (test_TriMatrix) {
	TriMatrix <int> M;
	M.put({35,42}, 3);
	M.put({1234,5678}, 5);
	M.put({91823749,-2793474}, 23);

	BOOST_CHECK (M.at({35,42}) == 3);
	BOOST_CHECK (M.at({1234,5678}) == 5);
	BOOST_CHECK (M.at({91823749,-2793474}) == 23);
	BOOST_CHECK (M.at({3,4}) == 0);
	BOOST_CHECK (M.at({981327,2371827}) == 0);
}

// Utility classes

BOOST_AUTO_TEST_CASE (test_coo) {
	coo z1 {2,3}, z2 {4,-1};
	BOOST_CHECK (z1+z2 == coo(6,2));
	BOOST_CHECK (z1-z2 == coo(-2,4));
	BOOST_CHECK (-z1 == coo(-2,-3));
	BOOST_CHECK (z1*3 == coo(6,9));
	BOOST_CHECK (z1/2 == coo(1,1));

	z1 += z2; z2 -= z1;
	BOOST_CHECK (cpx(z2) == cpx(-2,-3));
	BOOST_CHECK (norm(z2) == 13);
	BOOST_CHECK (sup(z2) == 3);
}

BOOST_AUTO_TEST_CASE (test_Color) {
	Color c (123);
	BOOST_CHECK (c == Color(123,123,123));
	BOOST_CHECK (c != Color(122,123,123));
}

BOOST_AUTO_TEST_CASE (test_ProgressBar) {
	auto l = Auto::tasks.size();
	{
		ProgressBar P (1e7);
		BOOST_CHECK (Auto::tasks.size() == l+1);
		BOOST_CHECK (Auto::tasks.back().active);
		for (int i=0; i<1e7; ++i) P.set(i);
	}
	BOOST_CHECK (!Auto::tasks.back().active);
}

BOOST_AUTO_TEST_CASE (test_Value) {
	Value v1 ("1"), v2 ("3.4");
	BOOST_CHECK (bool(v1));
	BOOST_CHECK (int(v1) == 1);
	BOOST_CHECK (unsigned(v1) == 1);
	BOOST_CHECK (long(v1) == 1);
	BOOST_CHECK (double(v2) == 3.4);

	v1 = "45"; BOOST_CHECK (int(v1) == 45);
}

// Globals and control structures

BOOST_AUTO_TEST_CASE (test_Hub) {
	const char * c_argv [] = { "cmd", "-s", "3", "-u" };
	char * argv[4]; for (int i=0; i<4; ++i) argv[i] = strdup (c_argv[i]);

	Hub H; H.init ("Title", 4, argv, "s=5,t=7,u,v");
	BOOST_CHECK (int(H['t']) == 7);
	BOOST_CHECK (int(H['s']) == 3);
	BOOST_CHECK (H['u']);
	BOOST_CHECK (!H['v']);

	for (int i=0; i<4; ++i) free(argv[i]);
}

// Math

BOOST_AUTO_TEST_CASE (test_math) {
	BOOST_CHECK (sign(3) == 1);
	BOOST_CHECK (sign(-2.0) == -1.0);
	BOOST_CHECK (sign(0) == 0);

	BOOST_CHECK (fact(3) == 6);
	BOOST_CHECK (binom(5,2) == 10);
	BOOST_CHECK (catalan(3) == 5);

	BOOST_CHECK (fabs(sin(pi_<double>())) < 1e-10);

	BOOST_CHECK (fabs(q_(cpx(1.0+1e-5))-q_(cpx(1.0)) - 1e-5*q_t(cpx(1.0))) < 1e-9);

	double e = sum<double> ([](int n) { auto out = 1.0/fact(n); return std::isinf(out) ? 0 : out; });
	BOOST_CHECK (fabs(log(e)-1) < 1e-6);
}

BOOST_AUTO_TEST_CASE (test_NumberTheory) {
	cln::default_float_format = cln::float_format(100);
	cln::cl_F z ("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
	ostringstream os; os << * (guess (z, 100));
	BOOST_CHECK (os.str() == "1*z^5 + -3*z^4 + 12*z^3 + -2*z^2 + 1*z + -7");
	ostringstream osr; osr << * (guess_r (z, 100));
	BOOST_CHECK (os.str() == osr.str());
	BOOST_CHECK (!guess(cln::pi(z),100));

	cln::cl_F z1 ("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
	cln::cl_F z2 ("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
	cln::cl_N zc = cln::complex (z1,z2);
	ostringstream osc; osc << * (guess_c (zc, 100));
	BOOST_CHECK (osc.str() == "1*z^4 + 3*z^3 + 4-5i*z^2 + -3*z + 1");
}

// Below is still to be done

BOOST_AUTO_TEST_CASE (test_Auto) {}

BOOST_AUTO_TEST_CASE (test_Automaton) {}

BOOST_AUTO_TEST_CASE (test_AutoWindow) {}

BOOST_AUTO_TEST_CASE (test_Bitmap) {}

BOOST_AUTO_TEST_CASE (test_CoarseImage) {}

BOOST_AUTO_TEST_CASE (test_Coloring) {}

BOOST_AUTO_TEST_CASE (test_Console) {}

BOOST_AUTO_TEST_CASE (test_Constellation) {}

BOOST_AUTO_TEST_CASE (test_Constellation0) {}

BOOST_AUTO_TEST_CASE (test_Constellation1) {}

BOOST_AUTO_TEST_CASE (test_cpx) {}

BOOST_AUTO_TEST_CASE (test_Cube) {}

BOOST_AUTO_TEST_CASE (test_Elliptic) {}

BOOST_AUTO_TEST_CASE (test_Figure) {}

BOOST_AUTO_TEST_CASE (test_Hypermap) {}

BOOST_AUTO_TEST_CASE (test_Hypermap_lib) {}

BOOST_AUTO_TEST_CASE (test_Image) {}

BOOST_AUTO_TEST_CASE (test_Lattice) {}

BOOST_AUTO_TEST_CASE (test_LinearAlgebra) {}

BOOST_AUTO_TEST_CASE (test_Map) {}

BOOST_AUTO_TEST_CASE (test_Minimizer) {}

BOOST_AUTO_TEST_CASE (test_Pairings) {}

BOOST_AUTO_TEST_CASE (test_Path) {}

BOOST_AUTO_TEST_CASE (test_Permutation) {}

BOOST_AUTO_TEST_CASE (test_Picture) {}

BOOST_AUTO_TEST_CASE (test_Point) {}

BOOST_AUTO_TEST_CASE (test_Polynomial) {}

BOOST_AUTO_TEST_CASE (test_Pov) {}

BOOST_AUTO_TEST_CASE (test_PRNG) {}

BOOST_AUTO_TEST_CASE (test_QuadTree) {}

BOOST_AUTO_TEST_CASE (test_RationalFraction) {}

BOOST_AUTO_TEST_CASE (test_Shape) {}

BOOST_AUTO_TEST_CASE (test_Spheroidal) {}

BOOST_AUTO_TEST_CASE (test_Toroidal) {}
