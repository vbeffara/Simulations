#include <vb/Coloring.h>
#include <vb/Hub.h>

using namespace vb;

Color wave (coo z) {
	static const int n = H['n'];
	int d = norm (z-coo(n/2,n/2));
	double useless = 0.01; for (int t=1; t<1000; ++t) useless = sin(useless); d += useless; // Just to take time
	return vb::Color ( 1+(2*d*((1<<8)-1))/(n*n) );
}

int main (int argc, char *argv[]) {
	H.init ("Sample",argc,argv,"n=500");
	Coloring S (H['n'],H['n'],wave);
	S.show(); S.pause(); S.output();
}
