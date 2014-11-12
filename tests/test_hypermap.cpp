#include <vb/Constellation.h>
#include <vb/Hub.h>
#include <vb/Minimizer.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Testing",argc,argv,"n=2,m=228,v,q,g=m_cube,p");

	auto M = HLib().at(H['g']);
	cout << M << endl;

	Constellation<double> C {M,H,H['n']};
	C.findn();
	if (H['v']) { C.belyi(); Image * I = C.draw(600); I->show(); I->pause(); I->hide(); delete I; }
	if (!H['q']) { C.belyi(); cout << endl << C; return 0; }

	#ifdef HAVE_COMPLEX_GMP
	Constellation<gmp100>	Cq = cconvert <double,gmp100> (C); Cq.findn(); Cq.belyi(); cout << endl << Cq;
	#else
	cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
	#endif
}
