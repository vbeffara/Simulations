#include <fftw3.h>
#include <limits>
#include <queue>
#include <vb/Image.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

class Info {
	public:
		Info (coo _z, coo _n, double _d, double _f) : z(_z), next(_n), d(_d), f(_f) {}
		bool operator< (const Info &o) const { return d > o.d; }

		coo z,next;
		double d,f;
	};

class QG : public Image { public:
	QG (Hub & H) : Image (1<<int(H['n']), 1<<int(H['n'])), I(w(),h(),Info({0,0},{0,0},0,0)), g(H['g']), n(H['n']) {
		if     	(H['w'] == "dyadic")  	fill_dyadic	(H['z']);
		else if	(H['w'] == "boolean") 	fill_boolean (H['z']);
		else if	(H['w'] == "white")   	fill_white ();
		else if	(H['w'] == "free")    	fill_free (H['z']);
		else if	(H['w'] == "gaussian")	fill_gaussian (H['z']);
		else { H.L->error ("Noise type {} unknown, no noise for you!", string(H['w'])); exit (1); }

		minf = maxf = I.at({0,0}).f; for (auto & u : I) { minf = min (minf,u.f); maxf = max (maxf,u.f); }
		H.L->info ("Renormalized field: min = {}, max = {}", minf/log(w()), maxf/log(w()));

		for (int j=0; j<h(); ++j)
			for (int i=0; i<w(); ++i) {
				coo z(i,j);
				put (z, Grey(255 * (I.at(z).f-minf)/(maxf-minf)));
				if (H['c']) put(z, HSV (int(at(z))>128 ? 0 : .5, .8, .8));
				I.at(z) = Info (z, z, numeric_limits<double>::infinity(), exp(g*I.at(z).f));
		}
	};

	void fill_dyadic (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<ww/ll; ++i) for (int j=0; j<hh/ll; ++j) {
				double g = prng.gaussian();
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) I.at(coo(x,y)).f += g;
			}
		}
	}

	void fill_boolean (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<ww/ll; ++i) for (int j=0; j<hh/ll; ++j) {
				double g = prng.uniform_real(-1,1);
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) I.at(coo(x,y)).f += g;
			}
		}
	}

	void fill_white () { for (auto & u : I) u.f = prng.gaussian() * sqrt((double)n); }

	void fill_free (int n0 = 0) {
		auto in = fftw_alloc_complex(ww*hh), out = fftw_alloc_complex(ww*hh);
		fftw_plan p = fftw_plan_dft_2d (ww, hh, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

		vector<double> sinarrayi(ww), sinarrayj(hh);
		for (int i=0; i<ww; ++i) sinarrayi[i] = sin(M_PI * i/ww);
		for (int j=0; j<hh; ++j) sinarrayj[j] = sin(M_PI * j/hh);

		for (int j=0; j<hh; ++j) for (int i=0; i<ww; ++i) {
			if ((i==0)&&(j==0)) continue;
			double norm = sqrt (ww*hh*(sinarrayi[i]*sinarrayi[i] + sinarrayj[j]*sinarrayj[j]));
			auto fij = cpx (prng.gaussian(),prng.gaussian()) * sqrt(M_PI/2) / norm;
			in[i+ww*j][0] = real(fij); in[i+ww*j][1] = imag(fij);
			if (norm > sqrt(ww*hh) * (1-n0/100.0)) { in[i+ww*j][0]=0; in[i+ww*j][1]=0; }
		}
		in[0][0] = 0; in[0][1] = 0;

		fftw_execute(p);
		for (int j=0; j<hh; ++j) for (int i=0; i<ww; ++i) I.at(coo(i,j)).f = out[i+ww*j][0];
		fftw_destroy_plan(p); fftw_free(in); fftw_free(out);
	}

	void fill_gaussian (double l = 10) {
		auto in = fftw_alloc_complex(ww*hh), out = fftw_alloc_complex(ww*hh);
		fftw_plan p = fftw_plan_dft_2d (ww, hh, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

		for (int64_t j=0; j<hh; ++j) for (int64_t i=0; i<ww; ++i) {
			int64_t ii = min (i,ww-i); int64_t jj = min (j,hh-j);
			double re = prng.gaussian (0, exp(-(ii*ii+jj*jj)/(l*l)));
			double im = 0*prng.gaussian (0, exp(-(ii*ii+jj*jj)/(l*l)));
			in[i+ww*j][0] = re; in[i+ww*j][1] = im;
		}

		fftw_execute(p);
		for (int j=0; j<hh; ++j) for (int i=0; i<ww; ++i) I.at(coo(i,j)).f = sign(out[i+ww*j][0]);
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
			for (int k=0; k<8; ++k) {
				coo nz = im.z + dz[k];
				if (!(contains(nz))) continue;
				Info & ni = I.at(nz);
				double nd = im.d + (k<4?.5:sqrt(.5)) * (im.f+ni.f);
				if (ni.d > nd) { ni.d = nd; ni.next = im.z; Q.push (ni); }
			}
		}
	}

	double radius () {
		double r = I.at({0,0}).d; for (int i=0; i<w(); ++i) {
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
		for (int y=0; y<h(); ++y) {
			for (int x=0; x<w(); ++x) {
				if (I.at(coo(x,y)).d <= r) put(coo(x,y), Color(0,0,127+at(coo(x,y)).b/2));
			}
		}
	}

	Array<Info> I;
	double g,minf, maxf;
	int n;
};

int main (int argc, char **argv) {
	H.init ("Random 2D geometry", argc, argv, "w=free,n=9,z=0,g=1,s=0,b,i,q,c");
	if (int s = H['s']) prng.seed(s);
	int n = H['n'], nn = 1<<n;

	QG img (H); if (!H['i']) img.show();

	if (!H['q']) {
		img.dijkstra();
		if (H['b']) img.ball ();
		for (int i=0; i<=nn-1; i+=1) { img.trace (coo(0,i)); img.trace (coo(nn-1,i)); img.trace (coo(i,0)); img.trace (coo(i,nn-1)); }
	}

	if (!H['i']) { img.pause(); } img.output();
}
