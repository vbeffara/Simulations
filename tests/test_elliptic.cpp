#include <vb/Constellation1.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Hypermap of genus 1",argc,argv,"n=3,m=228,v,w,q,g=lat_csquare,p");

	auto M = HLib().at(H['g']);
	cout << M << endl;

	Constellation1<double> C {M,H};
	C.findn();

	if (H['v']) { Image I (600,600,"Test Elliptic"); I.show(); C.draw(I); I.pause(); I.hide(); }
	if (!H['q']) { cout << endl << C; return 0; }

	#ifdef HAVE_COMPLEX_GMP
	Constellation1<gmp100> Cq = cconvert <double,gmp100> (C);
	Cq.findn(); cout << endl << Cq;
	if (H['w']) { Image I (600,600,"Test Elliptic (GMP)"); I.show(); Cq.draw(I); I.pause(); I.hide(); }
	#else
	cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
	#endif
}
