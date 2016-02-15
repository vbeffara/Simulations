#include <vb/Automaton.h>

using namespace vb;

const std::vector <Color> C { WHITE, BLACK, GREEN };

class Site { public:
	Site (int _s) : s(_s) {}
	operator Color() { return C[s]; }
	operator int() { return s; }
	int s;
};

class Contact : public Automaton<Site> { public:
	Contact (Hub & H) : Automaton<Site> (H['n'],1,H['p']) {
		for (auto &s : (*this)) if (prng.bernoulli(H['e'])) s = 2;

		add_rule (1,     	{ {},            	{{0,0}} 	});
		add_rule (H['l'],	{ {{0,1},{1,0}}, 	{{1,1}} 	});
		add_rule (H['l'],	{ {{0,1},{-1,0}},	{{-1,1}}	});
		add_rule (H['l'],	{ {{0,1},{1,2}}, 	{{1,1}} 	});
		add_rule (H['l'],	{ {{0,1},{-1,2}},	{{-1,1}}	});
		add_rule (H['m'],	{ {{0,2},{1,0}}, 	{{1,2}} 	});
		add_rule (H['m'],	{ {{0,2},{-1,0}},	{{-1,2}}	});

		if (H['j']) {
			add_rule (H['m'],	{ {{0,2},{1,1},{2,0}},  	{{2,2}} 	});
			add_rule (H['m'],	{ {{0,2},{-1,1},{-2,0}},	{{-2,2}}	});
		}
	}
};

int main (int argc, char ** argv) {
	H.init ("1D contact process",argc,argv,"n=1000,e=0,l=1.6494,m=100,j,p");
	Contact T(H); T.show(); while (true) T.swipe();
}
