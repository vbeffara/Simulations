#include <vb/CoarseImage.h>
#include <vb/Hub.h>
#include <map>

using namespace vb; using namespace std;

template <typename T> int sign (T val) { return (T(0) < val) - (val < T(0)); }

class Bouncy : public CoarseImage { public:
	Bouncy (int n, function<coo(coo)> j) : CoarseImage (n,n,pow(n,.25)), jump(j) { z0 = {n/2,n/2}; };

	void run () {
		coo z (0,0); while (contains(z)) {
			int already = at(z); put(z,1);
			z += already ? dz[prng.uniform_int(4)] : jump(z);
		}
	}

	function <coo(coo)> jump;
};

int main (int argc, char ** argv) {
	map <string, function<coo(coo)>> jumps;

	jumps.emplace ( "none", [](coo z) {                            	return coo(0,            	0);            	} );
	jumps.emplace ( "line", [](coo z) { double p = H['p'];         	return coo(- p*z.x,      	- p*z.y);      	} );
	jumps.emplace ( "idla", [](coo z) {                            	return coo(- z.x,        	- z.y);        	} );
	jumps.emplace ( "sign", [](coo z) {                            	return coo(- sign(z.x),  	- sign(z.y));  	} );
	jumps.emplace ( "sqrt", [](coo z) { double l = 1+sqrt(norm(z));	return coo(- z.x/sqrt(l),	- z.y/sqrt(l));	} );

	H.init ("Bouncy internal DLA", argc,argv, "n=2000,j=sqrt,p=.01");
	Bouncy B (H['n'],jumps.at(H['j']));
	B.show(); B.run(); B.pause();
}
