#include <vb/Coloring.h>
#include <vb/Hub.h>

using namespace vb;

int t;

Color M (cpx c) {
	cpx z=c;
	for (int i=0; i<t; ++i) {
		if (real(z)*real(z)+imag(z)*imag(z)>4) return Indexed(i);
		z = z*z + c;
	}
	return BLACK;
}

int main (int argc, char ** argv) {
	H.init ("Mandelbrot set", argc,argv, "n=800,t=1000");
	t = H['t'];
	Coloring C (cpx(-2.2,-1.5),cpx(1.2,1.5),H['n'],M);
	C.show(); return Fl::run();
}
