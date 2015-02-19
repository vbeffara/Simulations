#include <vb/Constellation1.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/ProgressBar.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Hypermap of genus 1",argc,argv,"m=228,v,w,q,n=0,g=lat_csquare,p,f=0,s=0,a,r=0");

	auto M = HLib().at(H['g']);
	int n = H['n']; if (n) M = H_genus1(n);
	int r = H['r']; if (r) M = H_artem(r);
	int f = H['f']; if (f) {
		if (int(H['s'])) prng.seed(int(H['s']));
		for (int i=0; i<f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
	}

	cout << M << endl;
	Constellation1<double> C {M,H};
	C.findn();

	if (!H['q']) { cout << endl << C; }
	if (H['v']) { Image I (800,800,"Test Elliptic"); I.show(); C.draw(I,H['a']); I.pause(); I.hide(); }

	if (H['q']) {
#ifdef HAVE_COMPLEX_GMP
		Constellation1<gmp100> Cq (C);
		Cq.findn(); cout << endl << Cq;
		if (H['w']) { Image I (600,600,"Test Elliptic (GMP)"); I.show(); Cq.draw(I); I.pause(); I.hide(); }
#else
		cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
#endif
	}
}
