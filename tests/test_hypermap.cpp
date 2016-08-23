#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/Hypermap_lib.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	H.init ("Hypermap of genus 0",argc,argv,"m=228,v,q,g=m_cube,p,a,f=0,s=0,u=0,d=1,D=0,o");

	auto M = HLib().at(H['g']);
	int u = H['u']; if (u) M = H_genus0(u);
	int f = H['f']; if (f) {
		if (int(H['s'])) prng.seed(int(H['s']));
		unsigned d = H['d'], D = H['D'];
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

	if (H['v']) {
		C.belyi(); auto bd = C.bounds();
		Coloring CC (bd.first,bd.second,800,[&](cpx z){ return HSV ((imag(C(z))>0)?0:.5, .8, .8); });
		CC.scale(1.5); CC.show(); if (H['o']) CC.output(); while (CC.visible()) { CC.update(); Fl::wait(); }
	}

	if (!H['q']) { C.belyi(); cout << endl << C; return 0; }

	Constellation0<real_t>	Cq (C); Cq.findn(); Cq.belyi();
	cout << endl << Cq;
}
