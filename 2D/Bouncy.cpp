#include <vb/CoarseImage.h>
#include <vb/Hub.h>
#include <vb/math.h>
#include <map>

using namespace vb; using namespace std;

class Bounces : public map <string, function<coo(coo)>> { public:
	Bounces () {
		emplace ( "none", [](coo z) {                                         	return coo(0,            	0);                                	} );
		emplace ( "line", [](coo z) { double p = H['p'];                      	return coo(- p*z.x,      	- p*z.y);                          	} );
		emplace ( "idla", [](coo z) {                                         	return coo(- z.x,        	- z.y);                            	} );
		emplace ( "sign", [](coo z) {                                         	return coo(- sign(z.x),  	- sign(z.y));                      	} );
		emplace ( "sig2", [](coo z) { int a = H['a'], k = sign(z.x*z.y);      	return coo(- sign(z.x),  	- sign(z.y)) * (1 + (a-1)*(1+k)/2);	} );
		emplace ( "soft", [](coo z) { int k = z.x > 2 * abs(z.y) ? H['a'] : 1;	return coo(- sign(z.x),  	- sign(z.y)) * k;                  	} );
		emplace ( "sqrt", [](coo z) { double l = 1+sqrt(norm(z));             	return coo(- z.x/sqrt(l),	- z.y/sqrt(l));                    	} );

		emplace ( "maxi", [](coo z) { return (abs(z.x)>abs(z.y)) ? coo(-sign(z.x),0) : coo(0,-sign(z.y)); } );

		emplace ( "dent", [](coo z) {	if (z.x+z.y > 2 * abs(z.x-z.y))	return (z.y>z.x) ? coo(-1,0) : coo(0,-1);
		                             	else                           	return (abs(z.y)>abs(z.x)) ? coo(0,-sign(z.y)) : coo(-sign(z.x),0);	} );

		emplace ( "octo", [](coo z) {	if (abs(z.x) > 2*abs(z.y))	return coo(-2*sign(z.x),0);
		                             	if (abs(z.y) > 2*abs(z.x))	return coo(0,-2*sign(z.y));
		                             	                          	return coo(-sign(z.x),-sign(z.y));	} );
	}
};

class Bouncy : public CoarseImage { public:
	Bouncy (int n, string j) : CoarseImage (n,n,pow(n,.25)), jump(Bounces().at(j)) { z0 = {n/2,n/2}; };

	void run () { coo z (0,0); while (contains(z)) { coo zz = at(z) ? dz[prng.uniform_int(4)] : jump(z); put(z,1); z += zz; } }

	function <coo(coo)> jump;
};

int main (int argc, char ** argv) {
	H.init ("Bouncy internal DLA", argc,argv, "n=1000,j=sqrt,p=.1,a=2");
	Bouncy B (H['n'],H['j']);
	B.show(); B.run(); B.output();
}
