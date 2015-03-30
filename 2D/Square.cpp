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

	pt reverse () {
		if     	(xf()==0) { xf()=1; xi()--; }
		else if	(xf()==1) { xf()=0; xi()++; }
		else if	(yf()==0) { yf()=1; yi()--; }
		else if	(yf()==1) { yf()=0; yi()++; }
		return (*this);
	}
};

ostream &operator<< (ostream &os, const pt p) { return os << p.x() << " " << p.y() << endl; }

pt nopoint (-1,-1,-1,-1);

typedef pair<pt,pt> ptpair;

pt geodesique (pt p, const Array<double> & o, ostream *os = 0) {
	set<pt> S;
	while (true) {
		if (! o.contains(coo(p.xi(),p.yi()))) break;
		if (os) (*os) << p;
		p.step(o);
		if (S.count(p)) break;
		S.insert(p);
	}
	if (os) (*os) << endl;

	if (! o.contains(coo(p.xi(),p.yi()))) return nopoint;

	pt p_min = p; p.step(o);
	while (p != p_min) {
		if (p < p_min) p_min = p;
		p.step(o);
	}

	return p_min;
}

pair<pt,pt> leaf (pt p, const Array<double> & o, ostream *os = 0) {
	pt p1 = geodesique (pt(p), o);
	p.reverse();
	pt p2 = geodesique (pt(p), o);

	if ((os != 0) && (p1 != p2)) {
		geodesique (pt(p), o, os);
		p.reverse();
		geodesique (p, o, os);
	}

	if (p1<p2) return pair<pt,pt>(p1,p2);
	else       return pair<pt,pt>(p2,p1);
}

set<pt> connections (const Array<double> & o, ostream *os = 0) {
	set<ptpair> S;
	set<pt> P;

	for (int i=0; i<o.ww; ++i) {
		for (int j=0; j<o.hh; ++j) {
			for (double x=0.01; x<1; x+=.01) {
				pair<pt,pt> pp = leaf (pt(i,j,x,0),o,os);

				if ((pp.first != nopoint) && (pp.second != nopoint)) {
					if (S.count(pp) == 0) {
						S.insert (pp);
						P.insert (pt(i,j,x,0));
					}
				}

				pp = leaf (pt(i,j,0,x),o,os);

				if ((pp.first != nopoint) && (pp.second != nopoint)) {
					if (S.count(pp) == 0) {
						S.insert (pp);
						P.insert (pt(i,j,0,x));
					}
				}
			}
		}
	}
	return P;
}

class Lamination : public Array<double> { public:
	Lamination (int n) : Array<double> (n,n,0) {
		for (auto & v : *this) v = tan(prng.uniform_real(0,M_PI));
	}
};

int main (int argc, char ** argv) {
	H.init ("Random lamination", argc,argv, "n=20,c");
	Lamination o (H['n']);

	for (int i=0; i<o.ww; ++i) for (int j=0; j<o.hh; ++j) {
		geodesique (pt(i,j,0,0),o,&cout);
		geodesique (pt(i,j,1,0),o,&cout);
		geodesique (pt(i,j,0,1),o,&cout);
		geodesique (pt(i,j,1,1),o,&cout);
	}

	if (H['c']) {
		set<pt> P = connections (o,&cout);
		set<pt> E;
		for (pt i : P) {
			pair<pt,pt> pp = leaf(i,o,0);
			E.insert (pp.first); E.insert (pp.second);
		}
		for (pt i : E) {
			geodesique (i,o,0);
			cerr << i << endl;
		}
	}
}
