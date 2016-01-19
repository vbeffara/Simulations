#include <vb/Array.h>
#include <vb/Hub.h>
#include <set>

using namespace vb; using namespace std;

using cooo = pair<int,double>;
using ptt = pair<cooo,cooo>;

class pt : public ptt { public:
	pt (int i, int j, double x, double y) : ptt(cooo(i,x), cooo(j,y)) {};

	int &   	xi	()      	{ return first.first;                         	}
	double &	xf	()      	{ return first.second;                        	}
	double  	x 	() const	{ return (double)first.first + first.second;  	}
	int &   	yi	()      	{ return second.first;                        	}
	double &	yf	()      	{ return second.second;                       	}
	double  	y 	() const	{ return (double)second.first + second.second;	}

	double dist2 (const pt o) const { double dx = x() - o.x(), dy = y() - o.y(); return dx*dx + dy*dy; }

	void step (const Array<double> & o) {
		int r=0;
		double oo = o.at(coo(xi(),yi()));

		if     	(yf()==1) { r = 1; oo = -1/oo; yf() = xf(); xf() = 0; }
		else if	(xf()==1) { r = 2; yf() = 1-yf(); xf() = 0; }
		else if	(yf()==0) { r = 3; oo = -1/oo; yf() = 1-xf(); xf() = 0; }

		int di=0, dj=0;

		if     	(oo < -yf()) 	{ xf() = -yf()/oo; yf() = 0; dj = -1; }
		else if	(oo < 1-yf())	{ xf() = 1; yf() = yf() + oo; di = 1; }
		else   	             	{ xf() = (1-yf())/oo; yf() = 1; dj = 1; }

		if     	(r==1) { int ti = di; di = dj; dj = -ti; double tx = xf(); xf() = yf(); yf() = 1-tx; }
		else if	(r==2) { di = -di; dj = -dj; xf() = 1-xf(); yf() = 1-yf(); }
		else if	(r==3) { int ti = di; di = -dj; dj = ti; double tx = xf(); xf() = 1-yf(); yf() = tx; }

		xf() -= di; xi() += di; yf() -= dj; yi() += dj;
	}

	void reverse () {
		if     	(xf()==0) { xf()=1; xi()--; }
		else if	(xf()==1) { xf()=0; xi()++; }
		else if	(yf()==0) { yf()=1; yi()--; }
		else if	(yf()==1) { yf()=0; yi()++; }
	}
};

ostream &operator<< (ostream &os, const pt p) { return os << p.x() << " " << p.y() << endl; }

pt nopoint (-1,-1,-1,-1);

using ptpair = pair<pt,pt>;

class Lamination : public Array<double> { public:
	Lamination (int n) : Array<double> (n,n,0) {
		for (auto & v : *this) v = tan(prng.uniform_real(0,M_PI));
	}

	pt geodesique (pt p, ostream *os = 0) const {
		set<pt> S;
		while (true) {
			if (!contains(coo(p.xi(),p.yi()))) break;
			if (os) (*os) << p;
			p.step(*this);
			if (S.count(p)) break;
			S.insert(p);
		}
		if (os) (*os) << endl;

		if (!contains(coo(p.xi(),p.yi()))) return nopoint;

		pt p_min = p; p.step(*this);
		while (p != p_min) {
			if (p < p_min) p_min = p;
			p.step(*this);
		}

		return p_min;
	}

	pair<pt,pt> leaf (pt p, ostream *os = 0) const {
		pt p1 = geodesique (p,os); p.reverse(); pt p2 = geodesique (p,os);
		if (p1<p2) return {p1,p2}; else return {p2,p1};
	}

	set<pt> connections () const {
		set<ptpair> S;
		set<pt> P;

		for (int i=0; i<ww; ++i) {
			for (int j=0; j<hh; ++j) {
				for (double x=0.01; x<1; x+=.01) {
					pair<pt,pt> pp = leaf (pt(i,j,x,0),0);

					if ((pp.first != nopoint) && (pp.second != nopoint)) {
						if (S.count(pp) == 0) { S.insert (pp); P.insert (pt(i,j,x,0)); }
					}

					pp = leaf (pt(i,j,0,x),0);

					if ((pp.first != nopoint) && (pp.second != nopoint)) {
						if (S.count(pp) == 0) { S.insert (pp); P.insert (pt(i,j,0,x)); }
					}
				}
			}
		}
		return P;
	}
};

int main (int argc, char ** argv) {
	H.init ("Random lamination", argc,argv, "n=20,c");
	Lamination o (H['n']);

	for (int i=0; i<o.ww; ++i) for (int j=0; j<o.hh; ++j) {
		o.geodesique (pt(i,j,0,0),&cout);
		o.geodesique (pt(i,j,1,0),&cout);
		o.geodesique (pt(i,j,0,1),&cout);
		o.geodesique (pt(i,j,1,1),&cout);
	}

	if (H['c']) {
		set<pt> P = o.connections();
		set<pt> E;
		for (pt i : P) { auto pp = o.leaf(i,0); E.insert (pp.first); E.insert (pp.second); }
		for (pt i : E) { o.geodesique (i,&cerr); }
	}
}
