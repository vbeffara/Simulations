#include <vb/Constellation1.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/ProgressBar.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Hypermap of genus 1",argc,argv,"m=228,v,w,q,n=0,g=lat_csquare,p,f=0,s=0");

	int n = H['n'];
	auto M = n ? H_genus1(n) : HLib().at(H['g']);
	int f = H['f']; if (f) {
		if (int(H['s'])) prng.seed(int(H['s']));
		for (int i=0; i<f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
	}

	cout << M << endl;
	Constellation1<double> C {M,H};
	C.findn();

	if (!H['q']) { cout << endl << C; }
	if (H['v']) { Image I (600,600,"Test Elliptic"); I.show(); C.draw(I); I.pause(); I.hide(); }

	if (H['q']) {
#ifdef HAVE_COMPLEX_GMP
		Constellation1<gmp100> Cq = cconvert <double,gmp100> (C);
		Cq.findn(); cout << endl << Cq;
		if (H['w']) { Image I (600,600,"Test Elliptic (GMP)"); I.show(); Cq.draw(I); I.pause(); I.hide(); }
#else
		cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
#endif
	}
}
