#include <vb/Hub.h>
#include <vb/Image.h>
#include <vb/cpx.h>
#include <fftw3.h>

using namespace vb; using namespace std;

class Field : public Array<double> { public:
	Field (Hub &H) : Array<double> (1<<int(H['n']),1<<int(H['n'])), n(H['n']) {
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

class QG : public Image { public: 
	QG (Hub & H) : Image (1<<int(H['n']), 1<<int(H['n']), "A dyadic GFF"), field(H), next(w(),h(),coo(w()/2,h()/2)), d(w(),h()) {
		double big = 0.0, min = field.at(0), max = field.at(0);

		for (auto & u : field) {
			if (u<min) min = u; if (u>max) max = u;
			u = exp(double(H['g']) * u) / w();
			big += u;
		}

		for (auto & u : d) u = big; d.at(coo(w()/2,h()/2)) = 0.0;
	};

	void find_geodesics () {
		unsigned int changed = 1; while (changed>0) {
			changed=0;

			for (int x=0; x<w(); ++x) {
				for (int y=0; y<h(); ++y) {
					coo z(x,y);
					bool flag = false;

					for (int k=0; k<4; ++k) {
						coo nz = z + dz[k];
						if (!(contains(nz))) continue;
						if ((d.at(nz) + field.at(z) < d.at(z))) { next.at(z) = nz; d.at(z) = d.at(nz) + field.at(z); flag=true; }
					}

					if (flag) ++changed;
				}
			}

			cerr << changed << "          \r";
		}
	}

	double radius () {
		double r = d.at(0); for (int i=0; i<w(); ++i) {
			r = min (r, d.at(coo(i,0))    );
			r = min (r, d.at(coo(0,i))    );
			r = min (r, d.at(coo(i,h()-1)));
			r = min (r, d.at(coo(w()-1,i)));
		}
		return r;
	}

	void trace (coo z) { while (at(z) != Color(255)) { put(z,255); z = next.at(z); } }

	Field field;
	Array<coo> next;
	Array<double> d;
};

int main (int argc, char **argv) {
	Hub H ("Random 2D geometry", argc, argv, "w=dyadic,n=9,z=0,g=1,s=0");
	if (int s = H['s']) prng.seed(s);
	int n = H['n'], nn = 1<<n;
	double g = H['g'];

	QG img (H);
	img.find_geodesics ();

	double radius = img.radius(); cerr << "Distance to the boundary : " << radius << endl;

	for (int i=0; i<nn; ++i)
		for (int j=0; j<nn; ++j) {
			double renorm = log(img.field.at(coo(i,j)))/(g*sqrt((double)n));
			int color = 32 * (2.0 + renorm);

			if (color>128) color=128;
			if (color<0) color=0;
			img.put(coo(i,j),color);
		}

	for (int i=0; i<=nn-1; i+=15) {
		img.trace (coo(0,i));
		img.trace (coo(nn-1,i));
		img.trace (coo(i,0));
		img.trace (coo(i,nn-1));
	}

	for (int x=0; x<nn; ++x) {
		for (int y=0; y<nn; ++y) {
			if (img.d.at(coo(x,y))<=radius)
				img.put(coo(x,y),127+img.at(coo(x,y))/2);
			else if (img.d.at(coo(x,y))-img.field.at(coo(x,y))<=radius)
				img.put(coo(x,y),0);
		}
	}

	img.output();
}
