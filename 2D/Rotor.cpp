#include <vb/Image.h>
#include <vb/CL_Parser.h>
#include <vb/ProgressBar.h>

using namespace vb;

const Color C[4] = { Color(255,0,0), Color(0,255,0), Color(0,0,255), Color(255,255,0) };

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=500");
	int n = CLP('n');

	Image img (n,n,"The Rotor-Router model");
	ProgressBar PB (n*n*.785398, 2);

	img.show();
	coo z(n/2,n/2);

	int surf=0;

	while (img.contains(z)) {
		Color c = img.at(z);
		if (c == Color(0,0,0,0)) {
			img.put (z,C[0]); z=coo(n/2,n/2); PB.set(++surf);
		} else for (int i=0; i<4; ++i)
			if (c == C[i]) { img.put (z,C[(i+1)%4]); z+=dz[i]; }
	}

	img.output_png();
}
