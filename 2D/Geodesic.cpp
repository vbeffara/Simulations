#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/cpx.h>
#include <fftw3.h>
#include <queue>

using namespace vb; using namespace std;

class Field : public Array<double> { public:
	Field (Hub &H) : Array<double> (1<<int(H['n']),1<<int(H['n']),0), n(H['n']) {
		if     	(H['w'] == "dyadic") 	fill_dyadic	(H['z']);
		else if	(H['w'] == "boolean")	fill_boolean (H['z']);
		else if	(H['w'] == "white")  	fill_white ();
		else if	(H['w'] == "free")   	fill_free ();
		else   	                     	cerr << "Noise type " << H['w'] << " unknown, no noise for you!" << endl;
	}

	void fill_dyadic (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<W/ll; ++i) for (int j=0; j<H/ll; ++j) {
				double g = prng.gaussian();
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) at(coo(x,y)) += g;
			}
		}
	}

	void fill_boolean (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<W/ll; ++i) for (int j=0; j<H/ll; ++j) {
				double g = prng.uniform_real(-1,1);
				for (int x=i*ll; x<(i+1)*ll; ++x) for (int y=j*ll; y<(j+1)*ll; ++y) at(coo(x,y)) += g;
			}
		}
	}

	void fill_white () { for (auto & u : *this) u = prng.gaussian() * sqrt((double)n); }

	void fill_free () {
		cpx *in = (cpx*) fftw_alloc_complex(W*H), *out = (cpx*) fftw_alloc_complex(W*H);
		fftw_plan p = fftw_plan_dft_2d (W, H, (fftw_complex*) in, (fftw_complex*) out, FFTW_FORWARD, FFTW_ESTIMATE);

		for (int i=0; i<W; ++i) for (int j=0; j<H; ++j) {
			if ((i==0)&&(j==0)) break;
			cpx z ( min(i,W-i) , min(j,H-j) );
			in[i+W*j] = cpx ( prng.gaussian() / abs(z), prng.gaussian() / abs(z) );
		}

		fftw_execute(p);
		for (int i=0; i<W; ++i) for (int j=0; j<H; ++j) put(coo(i,j),real(out[i+W*j]));
		fftw_destroy_plan(p); fftw_free(in); fftw_free(out);
	}

	int n;
};

class Info { 
	public: 
		Info (coo _z, coo _n, double _d, bool _done = false) : z(_z), next(_n), d(_d), done(_done) {} 
		bool operator< (const Info &o) const { return d > o.d; }

		coo z,next; 
		double d; 
		bool done;
	};

class QG : public Image { public: 
	QG (Hub & H) : Image (1<<int(H['n']), 1<<int(H['n']), H.title), field(H), I(w(),h(),Info(0,0,0)) {
		double big = 0, min = field.at(0), max = field.at(0), g = H['g'];

		for (auto & u : field) {
			if (u<min) min = u; if (u>max) max = u;
			u = exp(g*u);
			big += u;
		}

		coo mid (w()/2,h()/2);
		for (int i=0; i<w(); ++i)
			for (int j=0; j<h(); ++j) {
				coo z(i,j);
				I.at(z) = Info (z, mid, big);
				double c = log(field.at(z)) / g;
				int color = 255 * (c-min)/(max-min);
				put(z,color);
		}
		I.at(mid).d = 0.0; I.at(mid).done = true;
	};

	void dijkstra () {
		priority_queue<Info> Q;

		for (int i=0; i<4; ++i) { 
			coo z = coo(w()/2,h()/2), nz = z + dz[i]; 
			I.at(nz) = Info (nz,z,field.at(nz)); 
			Q.push (I.at(nz));
		}

		int left = w()*h()-1; while (left) {
			while (I.at(Q.top().z).done) Q.pop();
			Info im = Q.top(); Q.pop();
			I.at(im.z).done = true; --left;
			for (int k=0; k<4; ++k) {
				coo nz = im.z + dz[k];
				if (!(contains(nz))) continue;
				if (I.at(nz).done) continue;
				double nd = im.d + field.at(nz);
				if (I.at(nz).d > nd) { 
					I.at(nz).d = nd; 
					I.at(nz).next = im.z; 
					Q.push (I.at(nz));
				}
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

	void trace (coo z) { while (at(z) != Color(255)) { put(z,255); z = I.at(z).next; } }

	void ball () {
		double r = radius();
		for (int x=0; x<w(); ++x) {
			for (int y=0; y<h(); ++y) {
				if (I.at(coo(x,y)).d <= r) put(coo(x,y), Color(0,0,127+at(coo(x,y)).b/2));
			}
		}
	}

	Field field;
	Array<Info> I;
};

int main (int argc, char **argv) {
	Hub H ("Random 2D geometry", argc, argv, "w=dyadic,n=9,z=0,g=1,s=0,b,v");
	if (int s = H['s']) prng.seed(s);
	int n = H['n'], nn = 1<<n;

	QG img (H);
	if (H['v']) img.show();
	img.dijkstra();

	double radius = img.radius(); cerr << "Distance to the boundary : " << radius << endl;

	if (H['b']) img.ball ();

	for (int i=0; i<=nn-1; i+=15) { img.trace (coo(0,i)); img.trace (coo(nn-1,i)); img.trace (coo(i,0)); img.trace (coo(i,nn-1)); }

	img.output();
}
