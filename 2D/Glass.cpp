#include <vb/Image.h>

using std::vector, vb::H, vb::coo, vb::prng, vb::BLACK, vb::WHITE;

vector<bool> init_ok_none () { vector<bool> ok(256,0); for (int i=0; i<256; i++) ok[i]=1; return ok; }

vector<bool> init_ok_glass () {
	// Flippable iff at least two of the 4 neighbors are empty.
	vector<bool> ok(256,0);
	for (int i=0; i<256; i++) {
		int tmp = 0;
		if ((i&1) != 0) tmp++;
		if ((i&4) != 0) tmp++;
		if ((i&16) != 0) tmp++;
		if ((i&64) != 0) tmp++;
		ok[i] = (tmp<=2);
	}
	return ok;
}

vector<bool> init_ok_connect4 () {
	/* This is connectivity-conditioning, on  the square lattice (which is
	 * a bit awkward, in particular it is not ergodic for the wrong reason
	 * that the lattice  and dual lattice look different  - the checkboard
	 * configuration is fixed ...). */

	vector<bool> ok(256,0);
	vector<int> tmp1(8), tmp2(8);

	for (int i=0; i<256; i++) {
		for (int j=0; j<8; j++) {
			tmp1[j] = (i&(1<<j)) / (1<<j);
			tmp2[j] = 1<<j;
		}

		if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
		if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];
		if ( (tmp1[4]==tmp1[6]) && (tmp1[4]==tmp1[5]) ) tmp2[6]=tmp2[4];
		if ( (tmp1[6]==tmp1[0]) && (tmp1[6]==tmp1[7]) ) tmp2[0]=tmp2[6];
		if ( (tmp1[0]==tmp1[2]) && (tmp1[0]==tmp1[1]) ) tmp2[2]=tmp2[0];
		if ( (tmp1[2]==tmp1[4]) && (tmp1[2]==tmp1[3]) ) tmp2[4]=tmp2[2];

		int nb1 = (tmp1[0]*tmp2[0]) | (tmp1[2]*tmp2[2]) | (tmp1[4]*tmp2[4]) | (tmp1[6]*tmp2[6]);
		int nb2 = ((1-tmp1[0])*tmp2[0]) | ((1-tmp1[2])*tmp2[2]) | ((1-tmp1[4])*tmp2[4]) | ((1-tmp1[6])*tmp2[6]);

		nb1 = (nb1&1) + ((nb1&4)/4) + ((nb1&16)/16) + ((nb1&64)/64);
		nb2 = (nb2&1) + ((nb2&4)/4) + ((nb2&16)/16) + ((nb2&64)/64);

		ok[i] = (((nb1>1)||(nb2>1)  ? 0 : 1) != 0);
	}
	return ok;
}

vector<bool> init_ok_connect6 () {
	// Connectivity-conditioning on the triangular lattice.

	vector<bool> ok(256,0);
	int tmp[6];

	for (int i=0; i<256; i++) {
		tmp[0]=(i&1)/1;
		tmp[1]=(i&2)/2;
		tmp[2]=(i&4)/4;
		tmp[3]=(i&16)/16;
		tmp[4]=(i&32)/32;
		tmp[5]=(i&64)/64;

		int nb=0;
		if (tmp[1]!=tmp[0]) nb++;
		if (tmp[2]!=tmp[1]) nb++;
		if (tmp[3]!=tmp[2]) nb++;
		if (tmp[4]!=tmp[3]) nb++;
		if (tmp[5]!=tmp[4]) nb++;
		if (tmp[0]!=tmp[5]) nb++;

		ok[i] = ((nb<=2 ? 1 : 0) != 0);
	}
	return ok;
}

class Glass : public vb::Image { public:
	explicit Glass (int n) : vb::Image(n,n) {
		std::map <std::string,std::function<vector<bool>()>> init;
		init.emplace ("none", init_ok_none);
		init.emplace ("glass", init_ok_glass);
		init.emplace ("connect4", init_ok_connect4);
		init.emplace ("connect6", init_ok_connect6);
		ok = init[H['c']]();

		fill ({0,0},BLACK); for (int i=0; i<n; i++) put(coo(i,n/2),WHITE); show();
	};

	void run () {
		int n = w(); double p = H['p'];

		for (int i=0; i<2000*n*n; i++) {
			int x = 1 + (prng()%(n-2));
			int y = 1 + (prng()%(n-2));
			int nb = 0;
			if (at(coo(x+1,y  )) != BLACK) nb += 1;
			if (at(coo(x+1,y+1)) != BLACK) nb += 2;
			if (at(coo(x  ,y+1)) != BLACK) nb += 4;
			if (at(coo(x-1,y+1)) != BLACK) nb += 8;
			if (at(coo(x-1,y  )) != BLACK) nb += 16;
			if (at(coo(x-1,y-1)) != BLACK) nb += 32;
			if (at(coo(x  ,y-1)) != BLACK) nb += 64;
			if (at(coo(x+1,y-1)) != BLACK) nb += 128;

			if (ok[nb]) {
				if (prng.bernoulli(p)) {
					if (at(coo(x,y)) == BLACK) {
						int tmp = int(at(coo(x+1,y)));
						tmp    |= int(at(coo(x-1,y)));
						tmp    |= int(at(coo(x,y+1)));
						tmp    |= int(at(coo(x,y-1)));
						if (tmp==0) tmp=85;
						put(coo(x,y),vb::Grey(tmp));
					}
				} else put(coo(x,y),BLACK);
			}
		}
	}

	vector<bool> ok;
};

int main (int argc, char ** argv) {
	H.init ("Glassy Glauber dynamics for percolation",argc,argv,"n=300,p=.5,c=none");
	Glass img (H['n']); img.run();
}
