#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <fftw3.h>

using namespace vb; using namespace std;

class Field : public Bitmap<double> { public:
	Field (Hub &H) : Bitmap<double> (1<<int(H['n']),1<<int(H['n']),"Random field"), n(H['n']) {
		if     	(H['w'] == "dyadic") 	fill_dyadic (H['z']);
		else if	(H['w'] == "boolean")	fill_boolean (H['z']);
		else if	(H['w'] == "white")  	fill_white ();
		else if	(H['w'] == "free")   	fill_free ();
		else   	                     	cerr << "Noise type " << H['w'] << " unknown, no noise for you!" << endl;
	}

	void fill_dyadic (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<w()/ll; ++i)
				for (int j=0; j<w()/ll; ++j) {
					double g = prng.gaussian();
					for (int x=i*ll; x<(i+1)*ll; ++x)
						for (int y=j*ll; y<(j+1)*ll; ++y)
							at(coo(x,y)) += g;
				}
		}
	}

	void fill_boolean (int n0) {
		for (int l=n-1; l>=n0; --l) {
			int ll = 1<<l;
			for (int i=0; i<w()/ll; ++i)
				for (int j=0; j<w()/ll; ++j) {
					double g = prng.uniform_real(-1,1);
					for (int x=i*ll; x<(i+1)*ll; ++x)
						for (int y=j*ll; y<(j+1)*ll; ++y)
							at(coo(x,y)) += g;
				}
		}
	}

	void fill_white () { for (auto & u : *this) u = prng.gaussian() * sqrt((double)n); }

	void fill_free () {
		fftw_complex *in  = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w() * w());
		fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * w() * w());
		fftw_plan p = fftw_plan_dft_2d (w(), w(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);

		for (int i=0; i<w(); ++i) {
			for (int j=0; j<w(); ++j) {
				if ((i==0)&&(j==0)) break;
				double size = 0;
				if (i<w()/2) size += i*i; else size += (w()-i)*(w()-i);
				if (j<w()/2) size += j*j; else size += (w()-j)*(w()-j);
				size = sqrt(size);
				in[i+w()*j][0] = prng.gaussian() / size;
				in[i+w()*j][1] = prng.gaussian() / size;
			}
		}

		fftw_execute(p);
		for (int i=0; i<w(); ++i) for (int j=0; j<w(); ++j) put(coo(i,j),out[i+w()*j][0]);
		fftw_destroy_plan(p); fftw_free(in); fftw_free(out);
	}

	int n;
};

void trace (Image &img, vector<int> &direction, int x, int y) {
	int nn = img.w();

	while (img.at(coo(x,y)) != Color(255)) {
		img.put (coo(x,y),255);
		int xy = direction[x+nn*y];
		x = xy%nn;
		y = xy/nn;
	}
}

void find_geodesics (const Field & field, vector<double> &distance, vector<int> &direction, int nn) {
	unsigned int changed = 1; while (changed>0) {
		changed=0;

		for (int x=0; x<nn; ++x) {
			for (int y=0; y<nn; ++y) {
				int xy = x+nn*y;
				bool flag = false;

				if ((x>0)    && (distance[xy-1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field.at(coo(x,y)); flag=true; }
				if ((x<nn-1) && (distance[xy+1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field.at(coo(x,y)); flag=true; }
				if ((y>0)    && (distance[xy-nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field.at(coo(x,y)); flag=true; }
				if ((y<nn-1) && (distance[xy+nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field.at(coo(x,y)); flag=true; }

				if (flag) ++changed;
			}
		}

		for (int x=nn-1; x>=0; --x) {
			for (int y=nn-1; y>=0; --y) {
				int xy = x+nn*y;
				bool flag = false;

				if ((x>0)    && (distance[xy-1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field.at(coo(x,y)); flag=true; }
				if ((x<nn-1) && (distance[xy+1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field.at(coo(x,y)); flag=true; }
				if ((y>0)    && (distance[xy-nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field.at(coo(x,y)); flag=true; }
				if ((y<nn-1) && (distance[xy+nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field.at(coo(x,y)); flag=true; }

				if (flag) ++changed;
			}
		}

		for (int x=0; x<nn; ++x) {
			for (int y=nn-1; y>=0; --y) {
				int xy = x+nn*y;
				bool flag = false;

				if ((x>0)    && (distance[xy-1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field.at(coo(x,y)); flag=true; }
				if ((x<nn-1) && (distance[xy+1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field.at(coo(x,y)); flag=true; }
				if ((y>0)    && (distance[xy-nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field.at(coo(x,y)); flag=true; }
				if ((y<nn-1) && (distance[xy+nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field.at(coo(x,y)); flag=true; }

				if (flag) ++changed;
			}
		}

		for (int x=nn-1; x>=0; --x) {
			for (int y=0; y<nn; ++y) {
				int xy = x+nn*y;
				bool flag = false;

				if ((x>0)    && (distance[xy-1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-1;  distance[xy] = distance[xy-1]  + field.at(coo(x,y)); flag=true; }
				if ((x<nn-1) && (distance[xy+1]  + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+1;  distance[xy] = distance[xy+1]  + field.at(coo(x,y)); flag=true; }
				if ((y>0)    && (distance[xy-nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy-nn; distance[xy] = distance[xy-nn] + field.at(coo(x,y)); flag=true; }
				if ((y<nn-1) && (distance[xy+nn] + field.at(coo(x,y)) < distance[xy])) { direction[xy] = xy+nn; distance[xy] = distance[xy+nn] + field.at(coo(x,y)); flag=true; }

				if (flag) ++changed;
			}
		}

		cerr << "\r" << changed << "       ";
	}
}

void explore (const Image &img, const vector<int> &direction, int x, int y, const vector<double> d, const vector<double> f) {
	int nn = img.w();

	double l = d[x+nn*y];

	cerr << endl;

	while ((x!=nn/2)||(y!=nn/2)) {
		cerr << l << " " << f[x+nn*y] << endl;
		l -= f[x+nn*y];
		int xy = direction[x+nn*y];
		x = xy%nn;
		y = xy/nn;
	}
}


int main (int argc, char **argv) {
	Hub H ("Random 2D geometry", argc, argv, "w=dyadic,n=8,z=0,g=1,s=0");
	if (int s = H['s']) prng.seed(s);
	int n = H['n'], nn = 1<<n;
	double g = H['g'];
	string noise = H['w'];

	Field field (H);

	double big = 0.0;
	double max = field.at(0);
	double min = field.at(0);

	for (auto & u : field) {
		if (u<min) min = u;
		if (u>max) max = u;

		u = exp(g * u) / nn;
		big += u;
	}

	vector<double> distance;
	for (int i=0; i<nn*nn; ++i) distance.push_back(big);
	distance[nn*(nn+1)/2] = 0.0;

	vector<int> direction;
	for (int i=0; i<nn*nn; ++i) direction.push_back(nn*(nn+1)/2);

	find_geodesics (field, distance, direction, nn);

	double radius = distance[0];
	for (int i=0; i<nn; ++i) {
		if (distance[i]<radius) radius=distance[i];
		if (distance[nn*i]<radius) radius=distance[i];
		if (distance[i+nn*(nn-1)]<radius) radius=distance[i];
		if (distance[nn-1+nn*i]<radius) radius=distance[i];
	}
	cerr << "Distance to the boundary : " << radius << endl;

	Image img (nn,nn,"A dyadic GFF");
	for (int i=0; i<nn; ++i)
		for (int j=0; j<nn; ++j) {
			double renorm = log(field.at(coo(i,j)))/(g*sqrt((double)n));
			int color = 32 * (2.0 + renorm);

			if (color>128) color=128;
			if (color<0) color=0;
			img.put(coo(i,j),color);
		}

	for (int i=0; i<=nn-1; i+=15) {
		trace (img, direction, 0, i);
		trace (img, direction, nn-1, i);
		trace (img, direction, i, 0);
		trace (img, direction, i, nn-1);
	}

	// for (int i=0; i<=nn-1; i+=15) {
	//	explore (img, direction, 0, i, distance, field);
	//	explore (img, direction, nn-1, i, distance, field);
	//	explore (img, direction, i, 0, distance, field);
	//	explore (img, direction, i, nn-1, distance, field);
	// }

	for (int x=0; x<nn; ++x) {
		for (int y=0; y<nn; ++y) {
			int i=x+nn*y;
			if (distance[i]<=radius)
				img.put(coo(x,y),127+img.at(coo(i,0))/2);
			else if (distance[i]-field.at(coo(x,y))<=radius)
				img.put(coo(x,y),0);
		}
	}
}
