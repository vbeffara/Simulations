/*
 * Coupling two realizations of percolation in an annulus, both conditioned to
 * have 4  arms and  not 6, but  conditioned to  touch different sides  of the
 * boundary with those 4  arms. Aim = they should more or  less agree inside a
 * small disk.
 */

#include <vb/Image.h>

using namespace vb; using namespace std;

const int RAD1 = 5, RAD2 = 10, RAD3 = 20, N = 2*RAD3;

int compute_diff (Image &c1, Image &c2, Image &d) {
	int n=0;
	for (int i=0; i<N; i++) {
		for (int j=0; j<N; j++) {
			if (c1.at(coo(i,j))==c2.at(coo(i,j))) { d.put(coo(i,j),0); }
			else if (c1.at(coo(i,j))>c2.at(coo(i,j))) { d.put(coo(i,j),GREEN); n++; }
			else { d.put(coo(i,j),RED); n++; }
		}
	}
	return n;
}

void compute_cpts (Image &c, Array<int> &cpts, int r1) {
	/*
	 * Determine the connected  components in the annulus of  radii r1<r2 around
	 * the center of the square, and put that in cpts.
	 */

	int t=0;
	for (int x=0; x<N; x++) {
		for (int y=0; y<N; y++) {
			if (c.at(coo(x,y)) == Color(1)) { cpts[x+N*y] = (++t); }
			else { cpts[x+N*y] = - (++t); }
		}
	}

	/*  hole in the middle ie for r<r1 */

	for (int x=-r1+1; x<r1-1; x++) for (int y=-r1+1; y<r1-1; y++) cpts[coo(x+N/2,y+N/2)]=0;

	/*  scan; on garde le plus grand |.| indice de chaque cluster */

	bool dirty = true; while (dirty==1) {
		dirty = 0;
		for (int x=0; x<N; x++) {
			for (int y=0; y<N; y++) {
				coo z (x,y);
				for (int i=0; i<6; ++i) {
					coo zz = z+dz[i]; if (!cpts.contains(zz)) continue;
					if ((cpts[z]>0) && (cpts[zz]>cpts[z])) { cpts[z] = cpts[zz]; dirty=1; }
					if ((cpts[z]<0) && (cpts[zz]<cpts[z])) { cpts[z] = cpts[zz]; dirty=1; }
				}
			}
		}
	}
}

int nbarms (Image &c, int r1, int r2, int sides) {
	/*
	 * Compute the number of connected  components spanning from r1 to r2 inside
	 * the annulus  (should be 0, 1  or even).
	 */

	static Array<int> expl (N,N); compute_cpts (c,expl,r1);

	static vector<char> table (2*N*N+1); for (int i=0; i<2*N*N+1; i++) table[i]=0;

	for (int i=-r2; i<r2; i++) {
		if (sides&1) table[N*N+expl[(N>>1)+i+N*((N>>1)-r2)]]=1;
		if (sides&2) table[N*N+expl[(N>>1)+i+N*((N>>1)+r2-1)]]=1;
		if (sides&4) table[N*N+expl[(N>>1)-r2+N*((N>>1)+i)]]=1;
		if (sides&8) table[N*N+expl[(N>>1)+r2-1+N*((N>>1)+i)]]=1;
	}

	int n=0; int k;
	for (int i=-r1; i<r1; i++) {
		k = expl[(N>>1)+i + N*((N>>1)-r1)]; if (table[N*N+k]==1) { table[N*N+k]=0; n++; }
		k = expl[(N>>1)+i + N*((N>>1)+r1-1)]; if (table[N*N+k]==1) { table[N*N+k]=0; n++; }
		k = expl[(N>>1)-r1 + N*((N>>1)+i)]; if (table[N*N+k]==1) { table[N*N+k]=0; n++; }
		k = expl[(N>>1)+r1-1 + N*((N>>1)+i)]; if (table[N*N+k]==1) { table[N*N+k]=0; n++; }
	}

	return n;
}

bool test1 (Image &c) {
	/*
	 * 4 bras entre RAD1 et RAD3.
	 * 4 entre RAD1 et RAD2 et entre RAD2 et RAD3 i.e. jamais 6.
	 */

	if (nbarms(c,RAD1,RAD3,15)!=4) return false;
	if (nbarms(c,RAD2,RAD3,15)!=4) return false;
	if (nbarms(c,RAD1,RAD2,15)!=4) return false;
	return true;
}

bool test2 (Image &c) {
	/*
	 * test 1 plus exactement un bras sur le cote 1 entre RAD2 et RAD3.
	 */

	return (test1(c)==1) && (nbarms(c,RAD1,RAD3,6)==2);
}

void monte_carlo (Image &c1, Image &c2, Image &diff, int duration) {
	int x,y;
	for (int t=0; t<duration; t++) {
		while (true) {
			x = prng()%N; y = prng()%N;
			if ((x<(N>>1)-RAD1) || (x>=(N>>1)+RAD1) || (y<(N>>1)-RAD1) || (y>=(N>>1)+RAD1)) break;
		}

		auto z = prng.uniform_int(2);
		{ auto old = c1.at(coo(x,y)); c1.put(coo(x,y),z); if (!test1(c1)) c1.put(coo(x,y),old); }
		{ auto old = c2.at(coo(x,y)); c2.put(coo(x,y),z); if (!test2(c2)) c2.put(coo(x,y),old); }

		cerr << compute_diff (c1,c2,diff) << " \r"; diff.update();
	}
}

void pick (Image &c, function<bool(Image&)> test) {
	int n=0;
	while (true) {
		cerr << ++n << " \r";
		for (int x=0; x<N; x++) for (int y=0; y<N; y++) c.put (coo(x,y), prng.bernoulli(.5));

		for (int x=-RAD1+1; x<RAD1-1; x++) {
			for (int y=-RAD1+1; y<RAD1-1; y++) {
				c.put(coo((N>>1)+x,(N>>1)+y),0);
				c.put(coo((N>>1)+x,(N>>1)+y),0);
			}
		}

		if (test(c)) break;
	}
	cerr << endl;
}

int main (int, char **) {
	Image img1 (N,N); img1.label ("The first configuration");
	Image img2 (N,N); img2.label ("The second configuration");
	Image img  (N,N); img.label  ("The difference");

	pick (img1,test1); for (int i=0; i<N*N; i++) img2.put (coo(i,0), img1.at(coo(i,0)));

	img.show(); compute_diff(img1,img2,img); img.update();
	monte_carlo (img1,img2,img,N*N*N);
}
