#include <vb/Constellation.h>
#include <vb/Hub.h>
#include <vb/Minimizer.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=2,m=228,v,l,q,g=m_cube,p");

	auto M = HLib().at(H['g']);
	cout << M << endl;

	Constellation<double> C2 {M,H,H['n']}; if (H['v']) C2.show(); C2.findm();
	if (H['v']&&H['p']) { C2.belyi(); C2.slow=true; C2.img->update(); C2.img->pause(); } delete C2.img;
	if (!(H['q']||H['l'])) { C2.belyi(); cout << endl << C2; return 0; }

	Constellation<long double> C3 = cconvert <double, long double> (C2); if (H['v']) C3.show(); C3.findm(); delete C3.img;
	if (!H['q']) { C3.belyi(); cout << endl << C3; return 0; }

	Constellation<float128>	C4 = cconvert <long double,float128> (C3); if (H['v']) C4.show(); C4.findm(); delete C4.img;
	C4.belyi(); cout << endl << C4;
}
