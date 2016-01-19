#include <vb/Image.h>
#include <vb/CL_Parser.h>
#include <vb/ProgressBar.h>

using namespace vb;

const Color C[4] = { RED, GREEN, BLUE, YELLOW };

int main (int argc, char ** argv) {
	H.init ("Rotor-Router Model", argc, argv, "n=500");
	int n = H['n'];

	Image img (n,n);
	img.show();

	coo z(n/2,n/2);
	while (img.contains(z)) {
		Color c = img.at(z);
		if (c == Color(0)) {img.put (z,C[0]); z=coo(n/2,n/2); }
		else for (int i=0; i<4; ++i)
			if (c == C[i]) { img.put (z,C[(i+1)%4]); z+=dz[i]; }
	}

	img.output_png();
}
