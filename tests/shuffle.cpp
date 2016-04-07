// Initial code in Python by Sunil Chhita, ported to C++ by VB.

#include <vb/Array.h>
#include <vb/Hub.h>
#include <fstream>

using namespace vb; using namespace std;

int pgcd (int a, int b) { while (b) { int c = a%b; a=b; b=c; } return a; }

auto weights (int m, vector<vector<double>> p) {
	int w = p.size(), h = p[0].size(), wh = w*h/pgcd(w,h);
	Array<double> A (2*m*wh,2*m*wh);
	for (int i=0; i<2*m*wh; ++i) for (int j=0; j<2*m*wh; ++j) A[coo(i,j)] = p [(i/2)%w] [(j/2)%h];
	return A;
}

auto twoperiodic    (int m, double a, double b)          	{ return weights (m, {{a,b},{b,a}}); }
auto threeperiodic  (int m, double a, double b, double c)	{ return weights (m, {{a,b,c},{b,c,a},{c,a,b}}); }

auto d3p (const Array<double> & x1) {
	vector<vector<vector<vector<double>>>> A;
	int n = x1.ww; A.resize (n/2);

	A[0].resize (n, vector<vector<double>> (n));
	for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) if (double w = x1[coo(i,j)]) A[0][i][j] = {w,0}; else A[0][i][j] = {1,1};

	for (int k=0; k<n/2-1; ++k) {
		A[k+1].resize(n-2*k-2, vector<vector<double>> (n-2*k-2));
		for (int i=0; i<n-2*k-2; ++i) {
			for (int j=0; j<n-2*k-2; ++j) {
				const vector<double> & a1 = A[k][i+2*(i%2)][j+2*(j%2)];
				double a20, a21;
				if (A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] == A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1]) {
					a20 = A[k][i+2*(i%2)][j+2*(j%2)][0] * A[k][i+1][j+1][0] + A[k][i+2*(i%2)][j+1][0] * A[k][i+1][j+2*(j%2)][0];
					a21 = A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1];
				} else if (A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] < A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1]) {
					a20 = A[k][i+2*(i%2)][j+2*(j%2)][0] * A[k][i+1][j+1][0];
					a21 = A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1];
				} else {
					a20 = A[k][i+2*(i%2)][j+1][0] * A[k][i+1][j+2*(j%2)][0];
					a21 = A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1];
				}
				A[k+1][i][j] = { a1[0]/a20, a1[1]-a21 };
			}
		}
	}
	return A;
}

auto probs (const Array<double> & x1) {
	auto a0 = d3p (x1);
	int n = a0.size();
	vector<vector<vector<double>>> A (n);

	for (int k=0; k<n; ++k) {
		A[k].resize (k+1, vector<double> (k+1));
		for (int i=0; i<k+1; ++i) {
			for (int j=0; j<k+1; ++j) {
				if (a0[n-k-1][2*i][2*j][1] + a0[n-k-1][2*i+1][2*j+1][1] > a0[n-k-1][2*i+1][2*j][1] + a0[n-k-1][2*i][2*j+1][1])
					A[k][i][j] = 0;
				else if (a0[n-k-1][2*i][2*j][1] + a0[n-k-1][2*i+1][2*j+1][1] < a0[n-k-1][2*i+1][2*j][1] + a0[n-k-1][2*i][2*j+1][1])
					A[k][i][j] = 1;
				else
					A[k][i][j] = (a0[n-k-1][2*i+1][2*j+1][0] * a0[n-k-1][2*i][2*j][0] /
						(a0[n-k-1][2*i+1][2*j+1][0] * a0[n-k-1][2*i][2*j][0] + a0[n-k-1][2*i+1][2*j][0]*a0[n-k-1][2*i][2*j+1][0]));
			}
		}
	}
	return A;
}

auto delslide (const vector<vector<int>> & x1) {
	int n = x1.size();
	vector<vector<int>> a0 (n+2, vector<int> (n+2));
	for (int i=0; i<n+2; ++i) {
		for (int j=0; j<n+2; ++j) if ((i==0) || (i==n+1) || (j==0) || (j==n+1)) a0[i][j] = 0; else a0[i][j] = x1[i-1][j-1];
	}
	for (int i=0; i<n/2; ++i) {
		for (int j=0; j<n/2; ++j) {
			if ((a0[2*i][2*j]==1) && (a0[2*i+1][2*j+1]==1)) { a0[2*i][2*j]=0; a0[2*i+1][2*j+1]=0; }
			else if ((a0[2*i][2*j+1]==1) && (a0[2*i+1][2*j]==1)) { a0[2*i+1][2*j]=0; a0[2*i][2*j+1]=0; }
		}
	}
	for (int i=0; i<n/2+1; ++i) {
		for (int j=0; j<n/2+1; ++j) {
			if (a0[2*i+1][2*j+1]==1) { a0[2*i][2*j]=1; a0[2*i+1][2*j+1]=0; }
			else if (a0[2*i][2*j]==1) { a0[2*i][2*j]=0; a0[2*i+1][2*j+1]=1; }
			else if (a0[2*i+1][2*j]==1) { a0[2*i][2*j+1]=1; a0[2*i+1][2*j]=0; }
			else if (a0[2*i][2*j+1]==1) { a0[2*i+1][2*j]=1; a0[2*i][2*j+1]=0; }
		}
	}
	return a0;
}

auto create (vector<vector<int>> & x0, const vector<vector<double>> & p) {
	int n = x0.size();
	for (int i=0; i<n/2; ++i) {
		for (int j=0; j<n/2; ++j) {
			if ((x0[2*i][2*j]==0) && (x0[2*i+1][2*j]==0) && (x0[2*i][2*j+1]==0) && (x0[2*i+1][2*j+1]==0)) {
				bool a1,a2,a3,a4;
				if (j>0) a1 = (x0[2*i][2*j-1]==0) && (x0[2*i+1][2*j-1]==0); else a1 = true;
				if (j<n/2-1) a2 = (x0[2*i][2*j+2]==0) && (x0[2*i+1][2*j+2]==0); else a2 = true;
				if (i>0) a3 = (x0[2*i-1][2*j]==0) && (x0[2*i-1][2*j+1]==0); else a3 = true;
				if (i<n/2-1) a4 = (x0[2*i+2][2*j]==0) && (x0[2*i+2][2*j+1]==0); else a4 = true;
				if (a1 && a2 && a3 && a4) {
					if (prng.bernoulli(p[i][j])) { x0[2*i][2*j]=1; x0[2*i+1][2*j+1]=1; }
					else { x0[2*i+1][2*j]=1; x0[2*i][2*j+1]=1; }
				}
			}
		}
	}
}

auto aztecgen (const Array<double> & xr) {
	auto x0 = probs(xr);
	int n = x0.size();
	vector<vector<int>> a1;
	if (prng.bernoulli(x0[0][0][0])) a1 = {{1,0},{0,1}}; else a1 = {{0,1},{1,0}};
	for (int i=0; i<n-1; ++i) { a1=delslide(a1); create(a1,x0[i+1]); }
	return a1;
}

auto height (vector<vector<int>> A) {
	int m = A.size()/2;
	vector<vector<int>> h (m+1, vector<int> (m+1));

	int z=0; h[0][0]=z;
	for (int x=0; x<m; ++x) { z += 4*A[2*x][0] + 4*A[2*x+1][0] - 2; h[x+1][0] = z; }
	for (int y=0; y<m; ++y) {
		int z = h[0][y] + 4*A[0][2*y] + 4*A[0][2*y+1] - 2; h[0][y+1] = z;
		for (int x=0; x<m; ++x) { z -= 4*A[2*x][2*y+1] + 4*A[2*x+1][2*y+1] - 2; h[x+1][y+1] = z; }
	}
	return h;
}

int main (int argc, char ** argv) {
	H.init ("Domino shuffle", argc,argv, "m=10,a=1,b=.5,c=.3,w=unif");
	int m = H['m'];

	Array<double> TP;
	if (H['w'] == "unif") TP = weights (m, {{1}}); else
	if (H['w'] == "two") TP = twoperiodic (m,H['a'],H['b']); else
	if (H['w'] == "three") TP = threeperiodic (m,H['a'],H['b'],H['c']); else {
		cerr << "No such weight, \"" << string(H['w']) << "\".\n"; exit(1);
	}
	auto A1 = aztecgen(TP);

	string asyname = H.dir + H.title + ".asy"; cerr << "Asymptote file:   " << asyname << endl; ofstream asy (asyname);
	for (int y=0; y<A1.size(); ++y) for (int x=0; x<A1[0].size(); ++x) if (A1[y][x]) {
		double eps = ((x+y)%2) ? .5 : -.5;
		asy << "draw ((" << x-.5 << "," << y-eps << ")--(" << x+.5 << "," << y+eps << "), gray (" << TP[coo(x,y)]/1.1 << "));" << endl;
	}

	string datname = H.dir + H.title + ".dat"; cerr << "Height data file: " << datname << endl; ofstream dat (datname);
	for (auto & l : height(A1)) { for (auto h : l) dat << h << " "; dat << endl; }
}
