#include <vb/Hub.h>
#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace std; using namespace vb;

class Site { public:
	Site (int age) : a(age)	{ }
	operator int ()        	{ return a; }
	operator Color ()      	{ return a==0 ? BLACK : Indexed(a); }
	int a;
};

class ACP : public Bitmap<Site> { public:
	ACP (Hub & H) : Bitmap<Site> (int(H['n']), int(H['n']), H.title) { put (coo(w()/2,h()/2),1); }
	void run() {
		while (true) {
			coo z = rand(); int action = prng.discrete (P);
			if (action == 0)                	{ at(z)=0; }
			else if (action == 1)           	{ int a = at(z); if (a>0) put(z, min(a+1,5)); }
			else if (action-1 <= int(at(z)))	{ coo nz = z + dz[prng.uniform_int(4)]; if (atp(nz) == 0) putp(nz,1); }
		}
	}
	const vector<double> P { .005, .095, .2, .3, .4 };
};

int main (int argc, char ** argv) {
	Hub H ("Contact process with aging", argc, argv, "n=400");
	ACP A (H); A.show(); A.run();
}
