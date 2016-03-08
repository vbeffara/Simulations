#include <vb/Coloring.h>
#include <vb/Constellation1.h>
#include <vb/Hypermap_lib.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	H.init ("Hypermap of genus 1",argc,argv,"m=228,v,w,q,n=0,g=lat_csquare,p,f=0,s=0,a,b,r=0");

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
	if (H['v']) {
		// Image I (800,800); I.show(); C.draw(I,H['a'],H['b']); I.pause(); I.hide();
		auto bd = C.bounds();
		Coloring CC (bd.first,bd.second,800,[&](cpx z){ return HSV ((imag(C(z))>0)?0:.5, .8, .8); });
		CC.scale(1.5); CC.show(); while (CC.visible()) { CC.update(); Fl::wait(); }
	}

	if (H['q']) {
		Constellation1<gmp100> Cq (C);
		Cq.findn(); cout << endl << Cq;
	}
}
