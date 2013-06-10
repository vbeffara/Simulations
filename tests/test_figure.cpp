/// @file
#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace vb;

Color randomcolor() { return HSV (prng.uniform_real(0,1),1,1); }

int main (int argc, char **argv) {
	Figure F;
	for (int i=0; i<10; ++i)
		F.add (new Segment(	cpx(prng.uniform_real(-5,5),prng.uniform_real(-5,5)),
		                   	cpx(prng.uniform_real(-5,5),prng.uniform_real(-5,5)),
		                   	Indexed(i,.6,.9)));

	for (int i=1; i<=5; ++i) F.add (new Circle (cpx(0,0), i));

	F.show(); F.pause(); F.output ("output/test_figure");
}
