#include <vb/Image.h>

using namespace vb; using namespace std;

const Color LEFT(BLACK), RIGHT(WHITE);
const double betac = log(1+sqrt(2));

int main(int argc, char *argv[]) {
	H.init ("Ising model", argc,argv, "b=.88137359,n=300");
	int n = H['n'];
	double beta = H['b'];

	Image img(n,n);

	if (beta > betac)	{ for (int x=1; x<n-1; x++) for (int y=1; y<n-1; y++) img.put(coo(x,y), x<(n>>1)          	? LEFT : RIGHT);	}
	else             	{ for (int x=1; x<n-1; x++) for (int y=1; y<n-1; y++) img.put(coo(x,y), prng.bernoulli(.5)	? LEFT : RIGHT);	}

	for (int i=0; i<(n>>1); i++)	{ img.put(coo(i,0),LEFT);   	img.put(coo(i,n-1),LEFT); 	}
	for (int i=(n>>1); i<n; i++)	{ img.put(coo(i,0),RIGHT);  	img.put(coo(i,n-1),RIGHT);	}
	for (int i=0; i<n; i++)     	{ img.put(coo(0,i),LEFT);   	                          	}
	for (int i=0; i<n; i++)     	{ img.put(coo(n-1,i),RIGHT);	                          	}

	img.show();

	vector<double> p(5,0); for (int i=0; i<5; ++i) p[i] = exp(-i*beta);
	int nstep = 10 + n * 0.01 / fabs(beta-betac);

	for (int i=0; i<nstep; i++) {
		for (int x=1; x<n-1; x++) {
			for (int y=1; y<n-1; y++) {
				coo z(x,y); int s=0;
				for (int d=0; d<4; ++d) if (img.at(z) == img.at(z+dz[d])) s++; else s--;
				if ( (s<=0) || (prng.bernoulli(p[s])) ) img.put(coo(x,y),(255-img.at(z)));
			}
		}
	}

	img.output();
}
