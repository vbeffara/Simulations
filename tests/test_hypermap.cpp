#include <vb/Constellation0.h>
#include <vb/Hub.h>
#include <vb/Minimizer.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Hypermap of genus 0",argc,argv,"n=2,m=228,v,w,q,g=m_cube,p,a,b,f=0,s=0,u=0,d=1,D=0");

	auto M = HLib().at(H['g']);
	int u = H['u']; if (u) M = H_genus0(u);
	int f = H['f']; if (f) {
		if (int(H['s'])) prng.seed(int(H['s']));
		int d = H['d'], D = H['D'];
		bool bad = true; while (bad) {
			for (int i=0; i<f; ++i) M.flip(prng.uniform_int(M.sigma.size()));
			bad = false; f = 1;
			for (auto s : M.sigma.cycles()) {
				if (s.size()<d) bad = true;
				if ((D>0) && (s.size()>D)) bad = true;
			}
		}
	}
	cout << M << endl;

	Constellation0<double> C {M,H};

	if (H['v']) { C.belyi(); Image I (800,800,"Test Elliptic"); I.show(); C.draw(I,H['a'],H['b']); I.pause(); I.hide(); }
	if (!H['q']) { C.belyi(); cout << endl << C; return 0; }

	#ifdef HAVE_COMPLEX_GMP
	Constellation0<gmp100>	Cq (C); Cq.findn(); Cq.belyi();
	if (H['w']) { Image I (800,800,"Test Elliptic"); I.show(); Cq.draw(I,H['a'],H['b']); I.pause(); I.hide(); }
	cout << endl << Cq;
	#else
	cerr << "No std::complex<boost::numeric::whatever> support!" << endl;
	#endif
}
