#define BOOST_TEST_MODULE libvb unit tests
#define BOOST_TEST_DYN_LINK
#include <boost/lexical_cast.hpp>
#include <boost/test/unit_test.hpp>

#include <vb/Cluster.h>
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/Constellation1.h>
#include <vb/Cube.h>
#include <vb/Figure.h>
#include <vb/Hypermap_lib.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Pov.h>
#include <vb/ProgressBar.h>
#include <vb/Stream_lib.h>
#include <vb/TriMatrix.h>

using namespace vb; using namespace std; using namespace cln;

template <typename T> auto str (const T & t) { return boost::lexical_cast<string>(t); }

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

BOOST_AUTO_TEST_CASE (test_Cube) {
	Cube C (100,100,100);
	C.putp(C.rand(),1);
	int s=0; for (auto v : C) s += v;
	BOOST_TEST (s == 1);
	BOOST_TEST (Color(C.at({0,0})).a == 255);
	C.output_pov();
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

BOOST_AUTO_TEST_CASE (test_Cluster) {
	Cluster C;
	C.insert ({35,42});
	C.insert ({1234,5678});
	C.insert ({91823749,-2793474});
	C.remove ({1234,5678});

	BOOST_TEST (C.at({35,42}));
	BOOST_TEST (!C.at({1234,5678}));
	BOOST_TEST (C.at({91823749,-2793474}));
	BOOST_TEST (!C.at({3,4}));
	BOOST_TEST (!C.at({981327,2371827}));
}

// Utility classes

BOOST_AUTO_TEST_CASE (test_coo) {
	coo z1 {2,3}, z2 {4,-1};
	BOOST_TEST (z1+z2 == coo(6,2));
	BOOST_TEST (z1-z2 == coo(-2,4));
	BOOST_TEST (-z1 == coo(-2,-3));
	BOOST_TEST (z1*3 == coo(6,9));
	BOOST_TEST (z1/2 == coo(1,1));

	z1 += z2; z2 -= z1;
	BOOST_TEST (cpx(z2) == cpx(-2,-3));
	BOOST_TEST (norm(z2) == 13);
	BOOST_TEST (sup(z2) == 3);
	BOOST_TEST (str(z1) == "(6,2)");
}

BOOST_AUTO_TEST_CASE (test_Color) {
	Color c (123);
	BOOST_TEST (c == Color(123,123,123));
	BOOST_TEST (c != Color(122,123,123));
}

BOOST_AUTO_TEST_CASE (test_ProgressBar) {
	auto l = Auto::tasks.size();
	{
		ProgressBar P (1e8);
		BOOST_TEST (Auto::tasks.size() == l+1);
		BOOST_TEST (Auto::tasks.back().active);
		for (int i=0; i<1e8; ++i) P.set(i);
	}
	BOOST_TEST (!Auto::tasks.back().active);
}

BOOST_AUTO_TEST_CASE (test_Value) {
	Value v1 ("1"), v2 ("3.4");
	BOOST_TEST (bool(v1));
	BOOST_TEST (int(v1) == 1);
	BOOST_TEST (unsigned(v1) == 1);
	BOOST_TEST (long(v1) == 1);
	BOOST_TEST (double(v2) == 3.4);

	v1 = "45"; BOOST_TEST (int(v1) == 45);
}

// Globals and control structures

BOOST_AUTO_TEST_CASE (test_Hub) {
	vector<string> argv_ { "cmd", "-s", "3", "-u" };
	vector<char*> argv; argv.reserve(argv_.size()); for (auto & s : argv_) argv.push_back(&s[0]);

	H.init ("Title", 4, argv.data(), "s=5,t=7,u,v");
	BOOST_TEST (int(H['t']) == 7);
	BOOST_TEST (int(H['s']) == 3);
	BOOST_TEST (H['u']);
	BOOST_TEST (!H['v']);
}


BOOST_AUTO_TEST_CASE (test_Stream) {
	BOOST_TEST (size(cycles(3,6)) == 40);
	BOOST_TEST (size(permutations(7)) == fact(7));
	BOOST_TEST (size(hypermaps ({2,2,2},{2,2,2},{3,3})) == 1);

	int n=0; for (const auto & c : partitions(6)) n += size (permutations(c));
	BOOST_TEST (n == fact(6));
}

// Math

BOOST_AUTO_TEST_CASE (test_cpx) {
	cpx z (1.0,2.3);
	BOOST_TEST (str(z) == "(1,2.3)");
	cpxint zz = cln::complex (1,-3);
	BOOST_TEST (str(zz) == "1-3i");
}

BOOST_AUTO_TEST_CASE (test_math) {
	BOOST_TEST (sign(3) == 1);
	BOOST_TEST (sign(-2.0) == -1.0);
	BOOST_TEST (sign(0) == 0);

	BOOST_TEST (fact(3) == 6);
	BOOST_TEST (binom(5,2) == 10);
	BOOST_TEST (catalan(3) == 5);

	BOOST_TEST (abs(sin(pi_<double>())) < 1e-10);

	BOOST_TEST (abs(q_<double>(cpx(1.0+1e-5))-q_<double>(cpx(1.0)) - 1e-5*q_t<double>(cpx(1.0))) < 1e-9);

	auto e = sum<double> ([](int n) { auto out = 1.0/fact(n); return std::isinf(out) ? 0 : out; });
	BOOST_TEST (abs(log(e)-1) < 1e-6);
}

BOOST_AUTO_TEST_CASE (test_Permutation) {
	Permutation P1 = Transposition (4,0,1), P2 = Transposition (4,0,2), P = P1 * P2;
	Passport PP = P.passport();
	BOOST_TEST (PP[0].first == 3);
}

BOOST_AUTO_TEST_CASE (test_NumberTheory) {
	default_float_format = cln::float_format(100);
	cl_F z ("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
	BOOST_TEST (str(*(guess(z,100))) == "1*z^5 + -3*z^4 + 12*z^3 + -2*z^2 + 1*z + -7");

	cl_F z1 ("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
	cl_F z2 ("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
	cl_N zc = cln::complex (z1,z2);
	BOOST_TEST (str(*(guess_c(zc,100))) == "1*z^4 + 3*z^3 + 4-5i*z^2 + -3*z + 1");
	BOOST_TEST (str(*(guess_r(zc,100))) == "1*z^8 + 6*z^7 + 17*z^6 + 18*z^5 + 25*z^4 + -18*z^3 + 17*z^2 + -6*z + 1");

	BOOST_TEST (!guess(cln::pi(z),100));
}

BOOST_AUTO_TEST_CASE (test_LinearAlgebra) {
	Matrix<cl_N> m (3,3); for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	Vector<cl_N> v (3); for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	Vector<cl_N> x (3); x << -3.5,1.5,0;
	BOOST_TEST ((solve(m,v)-x).squaredNorm() < 1e-50);
}

BOOST_AUTO_TEST_CASE (test_Hypermap_lib) {
	BOOST_TEST (HLib().at("tripod_l").sigma.size() == 4);
	BOOST_TEST (!H_artem(12).is_simple(6));
	BOOST_TEST (H_genus0(15).is_triangulation());
	BOOST_TEST (H_genus1(18).is_graph());
}

BOOST_AUTO_TEST_CASE (test_Constellation0) {
	spdlog::set_level(spdlog::level::trace);
	auto M = HLib().at("m_dodecahedron");
	Constellation0<double> C {M,H};
	Constellation0<real_t> Cq (C);
	Cq.findn(); Cq.belyi();
	Polynomial<complex_t> Q; for (auto zd : Cq.f) for (unsigned j=0; j<zd.d; ++j) Q.add_root(zd.z);
	for (auto & x : Q) {
		auto xx = cln::complex (round1(realpart(x)), round1(imagpart(x)));
		if (abs(x - xx) < 1e-100) x = xx;
	}
	BOOST_TEST (str(Q) == " z^55 + -55 z^50 + 1205 z^45 + -13090 z^40 + 69585 z^35 + -134761 z^30 + -69585 z^25 + -13090 z^20 + -1205 z^15 + -55 z^10 + -1 z^5");
}

BOOST_AUTO_TEST_CASE (test_Constellation1) {
	auto M = HLib().at("lat_SV");
	Constellation1<double> C {M,H};
	Constellation1<real_t> Cq (C);
	Cq.findn();
	BOOST_TEST (str(*(guess_r(Cq.E.j(),80))) == "1*z^2 + -914416*z + 590816592");
}

BOOST_AUTO_TEST_CASE (test_Minimizer) {
	auto f = [](const Vector<double> &x, void *) {
		double o = 0;
		for (unsigned int i=0; i<400; ++i) o += (1 - cos(x[i]/(i+1)));
		return o;
	};

	auto g = [](const Vector<double> &x) {
		Vector<double> out(400);
		for (unsigned int i=0; i<400; ++i) out[i] = sin(x[i]/(i+1))/(i+1);
		return out;
	};

	auto fg = [](const Vector<double> &x, Vector<double> &g) {
		double o = 0;
		for (unsigned int i=0; i<400; ++i) {
			o += (1 - cos(x[i]/(i+1)));
			g[i] = sin(x[i]/(i+1))/(i+1);
		}
		return o;
	};

	Vector<double> x0(400); for (unsigned int i=0; i<400; ++i) x0[i] = cos(double(i));
	Vector<double> W0(400); for (unsigned int i=0; i<400; ++i) W0[i] = (i+1)*(i+1);

	Minimizer<double> M (400,fg);
	BOOST_TEST (M.minimize_qn (x0) < 1e-7);
	(void)f; (void)g;
}

// Displays, windows and such

BOOST_AUTO_TEST_CASE (test_Image) {
	Image img (256,256);
	img.show();
	for (int i=0; i<256; ++i)
		for (int j=0; j<256; ++j)
			img.put (coo(i,j), Color(i,j,(8*(i+j))%256));
	img.hide();
}

BOOST_AUTO_TEST_CASE (test_Coloring) {
	Coloring C (cpx(0,0), cpx(1,1), 500, [](cpx z){ return Indexed(real(z)*imag(z)*10); });
	C.scale(1.2); C.shift(cpx(.2,.3)); C.show(); C.hide();
}

BOOST_AUTO_TEST_CASE (test_Figure) {
	Figure F;
	for (int i=0; i<10; ++i)
		F.add (std::make_unique <Segment> (	cpx(prng.uniform_real(-5,5),prng.uniform_real(-5,5)),
		                                   	cpx(prng.uniform_real(-5,5),prng.uniform_real(-5,5)),
		                                   	Indexed(i,.6,.9)));

	for (int i=1; i<=5; ++i) F.add (std::make_unique <Circle> (cpx(0,0), i));
	F.add (std::make_unique <Dot> (cpx(0,0)));
	vector<cpx> zs1 { {0,1}, {1,1}, {1,0}}; F.add (std::make_unique <Polygon> (zs1));
	vector<cpx> zs2 { {0.1,1}, {1.1,1}, {1.1,0}}; F.add (std::make_unique <Path> (zs2));
	F.show(); F.hide();
}

BOOST_AUTO_TEST_CASE (test_Pov) {
	Pov_Box PB ({0.0,1.0,2.0}, {3.0,4.0,5.0});
}

// Below is still to be done

BOOST_AUTO_TEST_CASE (test_Auto) {}

BOOST_AUTO_TEST_CASE (test_Automaton) {}

BOOST_AUTO_TEST_CASE (test_AutoWindow) {}

BOOST_AUTO_TEST_CASE (test_Bitmap) {}

BOOST_AUTO_TEST_CASE (test_CoarseImage) {}

BOOST_AUTO_TEST_CASE (test_Console) {}

BOOST_AUTO_TEST_CASE (test_Constellation) {}

BOOST_AUTO_TEST_CASE (test_Elliptic) {}

BOOST_AUTO_TEST_CASE (test_Hypermap) {}

BOOST_AUTO_TEST_CASE (test_Lattice) {}

BOOST_AUTO_TEST_CASE (test_Map) {}

BOOST_AUTO_TEST_CASE (test_Pairings) {}

BOOST_AUTO_TEST_CASE (test_Path) {}

BOOST_AUTO_TEST_CASE (test_Picture) {}

BOOST_AUTO_TEST_CASE (test_Point) {}

BOOST_AUTO_TEST_CASE (test_Polynomial) {}

BOOST_AUTO_TEST_CASE (test_PRNG) {}

BOOST_AUTO_TEST_CASE (test_QuadTree) {}

BOOST_AUTO_TEST_CASE (test_RationalFraction) {}

BOOST_AUTO_TEST_CASE (test_Shape) {}

BOOST_AUTO_TEST_CASE (test_Spheroidal) {}

BOOST_AUTO_TEST_CASE (test_Toroidal) {}
