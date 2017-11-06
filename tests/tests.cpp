#define BOOST_TEST_MODULE libvb unit tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include <vb/Cluster.h>
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/Constellation1.h>
#include <vb/Figure.h>
#include <vb/Minimizer.h>
#include <vb/Pov.h>
#include <vb/ProgressBar.h>
#include <vb/Stream_lib.h>
#include <vb/TriMatrix.h>

using namespace vb; using namespace std; using namespace cln;

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
		                                   	Pen(Indexed(i,.6,.9))));

	for (int i=1; i<=5; ++i) F.add (std::make_unique <Circle> (cpx(0,0), i));
	F.add (std::make_unique <Dot> (cpx(0,0)));
	vector<cpx> zs1 { {0,1}, {1,1}, {1,0}}; F.add (std::make_unique <Polygon> (zs1));
	vector<cpx> zs2 { {0.1,1}, {1.1,1}, {1.1,0}}; F.add (std::make_unique <Path> (zs2));
	F.show(); F.hide();
}

BOOST_AUTO_TEST_CASE (test_Pov) {
	Pov_Box PB ({0.0,1.0,2.0}, {3.0,4.0,5.0});
}
