#include <vb/Bitmap.h>
#include <vb/Figure.h>

using namespace vb; using namespace std;

struct Spin {
	Spin (int s = 0) : s(s) {}
	operator Color() { return s ? Indexed((3+s)/2) : Grey(128); }
	int s;
};

class Ising : public Bitmap<Spin> { public:
	Ising (int n, double beta, int con) : Bitmap<Spin>(n,n), con(con), p(2*con+1) {
		for (auto & s : *this) s = 2*prng.bernoulli(.5)-1;
		if (con==6) {
			int m=2*n/3;
			for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) if ((j>m) || (i<j/2) || (i>m+j/2)) put(coo(i,j),0);
		}
		for (int c=-con; c<=con; ++c) p[c+con] = 1 / (1 + exp(-beta*c));
	};

	void run() {
		while (visible()) {
			coo z = rand(); if (!at(z).s) continue;
			int c = 0; for (int d=0; d<con; ++d) c += atp(z+dz[d]).s;
			put(z,2*prng.bernoulli(p[c+con])-1);
		}
	}

	void output_pdf (const std::string &s = "") {
		Figure F; cpx shift; vector<cpx> pattern;
		if (con==6) {
			shift = {-.5, sqrt(3.0)/2};
			for (int k=0; k<6; ++k) pattern.push_back (exp(cpx(0,(2*k+1)*M_PI/6))/sqrt(3.0));
		} else {
			shift = I;
			pattern = {{.5,.5}, {-.5,.5}, {-.5,-.5}, {.5,-.5}};
		}
		for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) {
			Color c = at(coo(i,j)); if (c==Spin(0)) continue;
			vector<cpx> p; p.reserve(pattern.size());
			for (auto zz : pattern) p.push_back (cpx(i) + cpx(j)*shift + zz);
			F.add (make_unique<Polygon> (p,Pen(BLACK,1,c,true)));
		}
		F.output_pdf(s);
	}

	void explore () {
		coo z0 = {w()/2, (con==6) ? h()/3 : h()/2};
		vector<coo> list {z0}; auto s = at(list.back()).s;
		while (list.size()) {
			auto z = list.back(); list.pop_back(); if (atp(z).s == 3) continue;
			putp(z,3); for (int d=0; d<con; ++d) if (atp(z+dz[d]).s==s) list.push_back(z+dz[d]);
		}
	}

	int con;
	vector<double> p;
};

int main (int argc, char ** argv) {
	H.init ("2D Ising model", argc, argv, "n=500,b=.7,c=4");
	Ising I (H['n'],H['b'],H['c']); I.show(); I.run(); I.explore(); I.output_pdf();
}
