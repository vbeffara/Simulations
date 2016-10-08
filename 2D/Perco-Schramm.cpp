
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace vb;

double omx = sqrt(3.0);

class Perco_Schramm : public Figure {
public:
	Perco_Schramm (Hub & H) : w(2*int(H['n'])), h(int(H['l']) ? H['l'] : w-1), mask(w*h,true) {
		for (int i=0; i < w/2; ++i)     cols.push_back (true);
		for (int i=0; i < w/2; ++i)     cols.push_back (false);
		for (int i=0; i < (w-1)*h; ++i) cols.push_back (prng.bernoulli(H['p']));
	}

	void tri_boundary () {
		for (long j=0; j<h; ++j) {
			for (long i=0; i<w; ++i)
				mask[i+w*j] = (i <= (w+j)/2) && (i >= (w-j)/2-1) && (j<h);
			cols[(w-j)/2 + w*j - 1] = true;
			cols[(w+j)/2 + w*j] = false;
		}
	}

	void rect_boundary () {
		for (long j=0; j<h-1; ++j) {
			cols[w*j] = true;
			cols[w*j+h] = false;
		}
	}

	void perc () {
		for (int i=0; i<w*h; ++i) if (mask[i]) {
			cpx xy = thepos(i);
			std::vector<cpx> coo;
			coo.push_back(xy + cpx(omx,1));  coo.push_back(xy + cpx(0,2));
			coo.push_back(xy + cpx(-omx,1)); coo.push_back(xy + cpx(-omx,-1));
			coo.push_back(xy + cpx(0,-2));   coo.push_back(xy + cpx(omx,-1));
			add (std::make_unique <Polygon> (coo, Pen(0, 1, cols[i] ? CYAN : YELLOW, true)));
		}
	}

	void walk () {
		int base = w/2-1, dir = 0;
		auto p = std::make_unique <Path> (std::vector<cpx>(0), Pen(BLUE,4));
		p->z.push_back (thepos(base) + cpx(omx,-1));
		while (((base+1)%w >= 0) && (base/w < h-1)) {
			seg (p.get(),base,dir,1);
			int thenext = follow (base, (dir+1)%6);
			if (cols[thenext])	{ base = thenext;	dir = (dir+5)%6; }
			else              	{                	dir = (dir+1)%6; }
			seg (p.get(),base,dir,5);
		}
		add(move(p));
	}

private:
	int w, h;
	std::vector<bool> cols, mask;

	cpx thepos (int i) { return cpx(omx*(((i/w)%2)+2*(i%w)) , 3*(i/w)); }

	int follow (int base, int dir) {
		static int fola[6] = { 1, w, w-1, -1, -w-1, -w };
		static int folb[6] = { 1, w+1, w, -1, -w, -w+1 };
		return ((base/w)%2 ? folb : fola) [dir] + base;
	}

	void seg (Path *p, int base, int dir, int rot) {
		cpx x1y1 = thepos(base);
		cpx x2y2 = thepos(follow(base,dir));
		cpx x3y3 = thepos(follow(base,(dir+rot)%6));
		p->z.push_back ((x1y1+x2y2+x3y3)/3.0);
	}
};

int main (int argc, char ** argv) {
	H.init ("Percolation exploration process", argc, argv, "n=28,l=55,p=.5,t");
	Perco_Schramm RS (H);
	if (H['t']) RS.tri_boundary(); else RS.rect_boundary();
	RS.perc(); RS.walk(); RS.show(); RS.pause(); RS.output();
}
