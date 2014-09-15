#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/ProgressBar.h>
#include <vb/cpx.h>
#include <fftw3.h>
#include <limits>
#include <queue>

using namespace vb; using namespace std;

class Info { 
	public: 
		Info (coo _z, coo _n, double _d, double _f) : z(_z), next(_n), d(_d), f(_f) {}
		bool operator< (const Info &o) const { return d > o.d; }

		coo z,next; 
		double d,f;
	};

class QG : public Image { public: 
	QG (Hub & H) : Image (1<<int(H['n']), 1<<int(H['n']), H.title), I(w(),h(),Info(0,0,0,0)), g(H['g']), n(H['n']) {
		if     	(H['w'] == "dyadic") 	fill_dyadic	(H['z']);
		else if	(H['w'] == "boolean")	fill_boolean (H['z']);
		else if	(H['w'] == "white")  	fill_white ();
		else if	(H['w'] == "free")   	fill_free ();
		else   	                     	cerr << "Noise type " << H['w'] << " unknown, no noise for you!" << endl;

		minf = maxf = I.at(0).f; for (auto & u : I) { minf = min (minf,u.f); maxf = max (maxf,u.f); }

		for (int i=0; i<w(); ++i)
			for (int j=0; j<h(); ++j) {
				coo z(i,j);
				put (z, 255 * (I.at(z).f-minf)/(maxf-minf));
				I.at(z) = Info (z, z, numeric_limits<double>::infinity(), exp(g*I.at(z).f));
		}
	};

	void fill_dyadic (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<W/ll; ++i) for (int j=0; j<H/ll; ++j) {
				double g = prng.gaussian();
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) I.at(coo(x,y)).f += g;
			}
		}
	}

	void fill_boolean (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<W/ll; ++i) for (int j=0; j<H/ll; ++j) {
				double g = prng.uniform_real(-1,1);
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) I.at(coo(x,y)).f += g;
			}
		}
	}

	void fill_white () { for (auto & u : I) u.f = prng.gaussian() * sqrt((double)n); }

	void fill_free () {
		cpx *in = (cpx*) fftw_alloc_complex(W*H), *out = (cpx*) fftw_alloc_complex(W*H);
		fftw_plan p = fftw_plan_dft_2d (W, H, (fftw_complex*) in, (fftw_complex*) out, FFTW_FORWARD, FFTW_ESTIMATE);

		for (int i=0; i<W; ++i) for (int j=0; j<H; ++j) {
			if ((i==0)&&(j==0)) break;
			cpx z ( min(i,W-i) , min(j,H-j) );
			in[i+W*j] = cpx ( prng.gaussian() / abs(z), prng.gaussian() / abs(z) );
		}

		fftw_execute(p);
		for (int i=0; i<W; ++i) for (int j=0; j<H; ++j) I.at(coo(i,j)).f = real(out[i+W*j]);
		fftw_destroy_plan(p); fftw_free(in); fftw_free(out);
	}

	void dijkstra () {
		coo mid (w()/2,h()/2);
		priority_queue<Info> Q; I.at(mid).d=0; Q.push(I.at(mid));
		ProgressBar PB (w()*h());

		for (int t=0; t < w()*h(); ++t) {
			PB.set (t);
			while (I.at(Q.top().z).d < Q.top().d) Q.pop();
			Info im = Q.top(); Q.pop();
			for (int k=0; k<4; ++k) {
				coo nz = im.z + dz[k];
				if (!(contains(nz))) continue;
				Info & ni = I.at(nz);
				double nd = im.d + ni.f;
				if (ni.d > nd) { ni.d = nd; ni.next = im.z; Q.push (ni); }
			}
		}
	}

	double radius () {
		double r = I.at(0).d; for (int i=0; i<w(); ++i) {
			r = min (r, I.at(coo(i,0)).d    );
			r = min (r, I.at(coo(0,i)).d    );
			r = min (r, I.at(coo(i,h()-1)).d);
			r = min (r, I.at(coo(w()-1,i)).d);
		}
		return r;
	}

	void trace (coo z, Color c = RED) { while (at(z) != c) { put(z,c); z = I.at(z).next; } }

	void ball () {
		double r = radius();
		for (int x=0; x<w(); ++x) {
			for (int y=0; y<h(); ++y) {
				if (I.at(coo(x,y)).d <= r) put(coo(x,y), Color(0,0,127+at(coo(x,y)).b/2));
			}
		}
	}

	Array<Info> I;
	double g,minf, maxf;
	int n;
};

int main (int argc, char **argv) {
	Hub H ("Random 2D geometry", argc, argv, "w=free,n=9,z=0,g=1,s=0,b,i");
	if (int s = H['s']) prng.seed(s);
	int n = H['n'], nn = 1<<n;

	QG img (H); if (!H['i']) img.show();

	img.dijkstra();
	if (H['b']) img.ball ();

	for (int i=0; i<=nn-1; i+=1) { img.trace (coo(0,i)); img.trace (coo(nn-1,i)); img.trace (coo(i,0)); img.trace (coo(i,nn-1)); }

	if (!H['i']) img.pause(); img.output();
}
