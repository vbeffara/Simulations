#include <vb/Constellation1.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Hypermap of genus 1",argc,argv,"n=3,m=228,v,q,g=lat_csquare,p");

	auto M = HLib().at(H['g']);
	cout << M << endl;

	Constellation1<double> C {M,H,H['n']};
	C.findn();

	if (H['v']) { Image * I = new Image (600,600,"Test Elliptic"); C.draw(*I); I->show(); I->pause(); I->hide(); delete I; }
	if (!H['q']) { cout << endl << C; return 0; }

	#ifdef HAVE_COMPLEX_GMP
	Constellation1<gmp100> Cq = cconvert <double,gmp100> (C);
	Cq.findn(); cout << endl << Cq;
	#else
	cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
	#endif
}
