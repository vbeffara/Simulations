#include <vb/CoarseImage.h>
#include <vb/Image.h>

using namespace vb; using namespace std;

class Bounces : public map <string, function<coo(coo)>> { public:
	Bounces () {
		emplace ( "none", [](coo)   {                                         	return coo(0,            	0);                                	} );
		emplace ( "line", [](coo z) { double p = H['p'];                      	return coo(- p*z.x,      	- p*z.y);                          	} );
		emplace ( "idla", [](coo z) {                                         	return coo(- z.x,        	- z.y);                            	} );
		emplace ( "sign", [](coo z) {                                         	return coo(- sign(z.x),  	- sign(z.y));                      	} );
		emplace ( "sig2", [](coo z) { long a = H['a'], k = sign(z.x*z.y);     	return coo(- sign(z.x),  	- sign(z.y)) * (1 + (a-1)*(1+k)/2);	} );
		emplace ( "soft", [](coo z) { int k = z.x > 2 * abs(z.y) ? H['a'] : 1;	return coo(- sign(z.x),  	- sign(z.y)) * k;                  	} );
		emplace ( "sqrt", [](coo z) { double l = 1+sqrt(norm(z));             	return coo(- z.x/sqrt(l),	- z.y/sqrt(l));                    	} );

		emplace ( "maxi", [](coo z) {                                        	return (abs(z.x)>abs(z.y)) ? coo(-sign(z.x),0) : coo(0,-sign(z.y)); } );
		emplace ( "max2", [](coo z) { int a = H['a'];                        	return (abs(z.x)>abs(z.y)) ? coo(-sign(z.x),0) : coo(0,-a*sign(z.y)); } );
		emplace ( "maxb", [](coo z) { double l = pow(norm(z),double(H['b']));	return (abs(z.x)>abs(z.y)) ? coo(-l*sign(z.x),0) : coo(0,-l*sign(z.y)); } );

		emplace ( "dent", [](coo z) {	if (z.x+z.y > 2 * abs(z.x-z.y))	return (z.y>z.x) ? coo(-1,0) : coo(0,-1);
		                             	else                           	return (abs(z.y)>abs(z.x)) ? coo(0,-sign(z.y)) : coo(-sign(z.x),0);	} );

		emplace ( "octo", [](coo z) {	if (abs(z.x) > 2*abs(z.y))        	return coo(-2*sign(z.x),0);
		                             	if (abs(z.y) > 2*abs(z.x))        	return coo(0,-2*sign(z.y));
		                             	return coo(-sign(z.x),-sign(z.y));	} );
	}
};

class Bouncy : public CoarseImage { public:
	Bouncy (int n, const string &j) : CoarseImage (n,n,pow(n,.25)), jump(Bounces().at(j)) {
		z0 = {n/2,n/2};
		if (H['g']) { tree = make_unique<Image> (2*n-1,2*n-1); tree->put(z0*2,WHITE); tree->show(); } else { show(); }
	};

	void run () {
		coo z (0,0); while (true) {
			coo nz = z;
			if (at(nz)) { nz += dz[prng.uniform_int(4)]; if (!contains(nz)) break; }
			if (!at(nz)) {
				if (H['g']) {
					Color c = tree->at((z+z0)*2);
					if ((c==BLACK) || (prng.bernoulli(H['u']))) c = HSV(prng.uniform_real(),1,1);
					if (H['i'] && (norm(nz)<10)) c = nz.y>0 ? Indexed(0) : Indexed(1);
					tree->put ((nz+z0)*2, c); tree->put (z+nz+z0*2, c);
				}
				put(nz,1); nz += jump(nz);
			}
			z = nz;
		}
	}

	function <coo(coo)> jump;
	unique_ptr <Image> tree;
};

int main (int argc, char ** argv) {
	H.init ("Internal DLA", argc,argv, "n=1000,j=idla,p=.1,a=2,b=.5,g,u=0,i");
	Bouncy B (H['n'],H['j']);
	B.run(); if (H['g']) B.tree->output(); else B.output();
}
