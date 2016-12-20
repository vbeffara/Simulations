// Initial code in Python by Sunil Chhita, ported to C++ by VB.
#include <vb/Hub.h>
#include <vb/Stream_lib.h>
#include <fstream>

using namespace vb; using namespace std;

auto weights (int m, const Array<double> & p) {
	int n = m*lcm(p.ww,p.hh);
	Array<double> A (n,n); for (auto z : coos(A)) A[z] = p.atp(z); return A;
}

auto dupe (const Array<double> & a) { Array<double> aa (2*a.ww,2*a.hh); for (auto z : coos(aa)) aa[z] = a[z/2]; return aa; }

auto twoperiodic  	(int m, double a, double b)          	{ return weights (m, dupe (Array<double> ({{a,b},{b,a}}))); }
auto threeperiodic	(int m, double a, double b, double c)	{ return weights (m, dupe (Array<double> ({{a,b,c},{b,c,a},{c,a,b}}))); }

auto threebytwo(int m, double b) { Array<double> p (6,6,1); p[coo(0,0)]=b; p[coo(4,2)]=b; p[coo(2,4)]=b; return weights (m,p); }

auto d3p (const Array<double> & x1) {
	int n = x1.ww; vector<Array<pair<double,double>>> A (n/2);

	A[0].resize(n,n); for (auto z : coos(x1)) if (double w = x1[z]) A[0][z] = {w,0}; else A[0][z] = {1,1};

	for (long k=0; k<n/2-1; ++k) {
		A[k+1].resize(n-2*k-2,n-2*k-2); for (auto z : coos(A[k+1])) {
			int i=z.x, j=z.y, ii = i+2*(i%2), jj = j+2*(j%2);
			double a20, a21;
			if (A[k][coo(ii,jj)].second + A[k][coo(i+1,j+1)].second == A[k][coo(ii,j+1)].second + A[k][coo(i+1,jj)].second) {
				a20 = A[k][coo(ii,jj)].first * A[k][coo(i+1,j+1)].first + A[k][coo(ii,j+1)].first * A[k][coo(i+1,jj)].first;
				a21 = A[k][coo(ii,jj)].second + A[k][coo(i+1,j+1)].second;
			} else if (A[k][coo(ii,jj)].second + A[k][coo(i+1,j+1)].second < A[k][coo(ii,j+1)].second + A[k][coo(i+1,jj)].second) {
				a20 = A[k][coo(ii,jj)].first * A[k][coo(i+1,j+1)].first;
				a21 = A[k][coo(ii,jj)].second + A[k][coo(i+1,j+1)].second;
			} else {
				a20 = A[k][coo(ii,j+1)].first * A[k][coo(i+1,jj)].first;
				a21 = A[k][coo(ii,j+1)].second + A[k][coo(i+1,jj)].second;
			}
			auto & a1 = A[k][coo(ii,jj)]; A[k+1][z] = { a1.first/a20, a1.second-a21 };
		}
	}
	return A;
}

auto probs (const Array<double> & x1) {
	auto a0 = d3p (x1); int n = a0.size(); vector<Array<double>> A (n);

	for (long k=0; k<n; ++k) {
		A[k].resize (k+1,k+1); for (auto z : coos(A[k])) {
			int i=z.x, j=z.y;
			if (a0[n-k-1][coo(2*i,2*j)].second + a0[n-k-1][coo(2*i+1,2*j+1)].second > a0[n-k-1][coo(2*i+1,2*j)].second + a0[n-k-1][coo(2*i,2*j+1)].second)
				A[k][z] = 0;
			else if (a0[n-k-1][coo(2*i,2*j)].second + a0[n-k-1][coo(2*i+1,2*j+1)].second < a0[n-k-1][coo(2*i+1,2*j)].second + a0[n-k-1][coo(2*i,2*j+1)].second)
				A[k][z] = 1;
			else
				A[k][z] = (a0[n-k-1][coo(2*i+1,2*j+1)].first * a0[n-k-1][coo(2*i,2*j)].first /
					(a0[n-k-1][coo(2*i+1,2*j+1)].first * a0[n-k-1][coo(2*i,2*j)].first + a0[n-k-1][coo(2*i+1,2*j)].first*a0[n-k-1][coo(2*i,2*j+1)].first));
		}
	}
	return A;
}

auto delslide (const Array<int> & x1) {
	int n = x1.ww; Array<int> a0 (n+2,n+2);
	for (int i=0; i<n+2; ++i)
		for (int j=0; j<n+2; ++j)
			a0[coo(i,j)] = ((i==0) || (i==n+1) || (j==0) || (j==n+1)) ? 0 : x1[coo(i-1,j-1)];
	for (int i=0; i<n/2; ++i) {
		for (int j=0; j<n/2; ++j) {
			if ((a0[coo(2*i,2*j)]==1) && (a0[coo(2*i+1,2*j+1)]==1)) { a0[coo(2*i,2*j)]=0; a0[coo(2*i+1,2*j+1)]=0; }
			else if ((a0[coo(2*i,2*j+1)]==1) && (a0[coo(2*i+1,2*j)]==1)) { a0[coo(2*i+1,2*j)]=0; a0[coo(2*i,2*j+1)]=0; }
		}
	}
	for (int i=0; i<n/2+1; ++i) {
		for (int j=0; j<n/2+1; ++j) {
			if (a0[coo(2*i+1,2*j+1)]==1) { a0[coo(2*i,2*j)]=1; a0[coo(2*i+1,2*j+1)]=0; }
			else if (a0[coo(2*i,2*j)]==1) { a0[coo(2*i,2*j)]=0; a0[coo(2*i+1,2*j+1)]=1; }
			else if (a0[coo(2*i+1,2*j)]==1) { a0[coo(2*i,2*j+1)]=1; a0[coo(2*i+1,2*j)]=0; }
			else if (a0[coo(2*i,2*j+1)]==1) { a0[coo(2*i+1,2*j)]=1; a0[coo(2*i,2*j+1)]=0; }
		}
	}
	return a0;
}

auto create (Array<int> & x0, const Array<double> & p) {
	int n = x0.ww;
	for (int i=0; i<n/2; ++i) {
		for (int j=0; j<n/2; ++j) {
			if ((x0[coo(2*i,2*j)]==0) && (x0[coo(2*i+1,2*j)]==0) && (x0[coo(2*i,2*j+1)]==0) && (x0[coo(2*i+1,2*j+1)]==0)) {
				bool a1,a2,a3,a4;
				if (j>0) a1 = (x0[coo(2*i,2*j-1)]==0) && (x0[coo(2*i+1,2*j-1)]==0); else a1 = true;
				if (j<n/2-1) a2 = (x0[coo(2*i,2*j+2)]==0) && (x0[coo(2*i+1,2*j+2)]==0); else a2 = true;
				if (i>0) a3 = (x0[coo(2*i-1,2*j)]==0) && (x0[coo(2*i-1,2*j+1)]==0); else a3 = true;
				if (i<n/2-1) a4 = (x0[coo(2*i+2,2*j)]==0) && (x0[coo(2*i+2,2*j+1)]==0); else a4 = true;
				if (a1 && a2 && a3 && a4) {
					if (prng.bernoulli(p[coo(i,j)])) { x0[coo(2*i,2*j)]=1; x0[coo(2*i+1,2*j+1)]=1; }
					else { x0[coo(2*i+1,2*j)]=1; x0[coo(2*i,2*j+1)]=1; }
				}
			}
		}
	}
}

auto aztecgen (const Array<double> & xr) {
	auto x0 = probs(xr); int n = x0.size(); Array<int> a1;
	if (prng.bernoulli(x0[0][coo(0,0)])) a1 = Array<int> ({{1,0},{0,1}}); else a1 = Array<int> ({{0,1},{1,0}});
	for (long i=0; i<n-1; ++i) { a1=delslide(a1); create(a1,x0[i+1]); }
	return a1;
}

auto height (const Array<int> & A) {
	int m = A.ww/2; Array<int> h (m+1,m+1,0); int z=0;
	for (int x=0; x<m; ++x) { z += 4*A[coo(2*x,0)] + 4*A[coo(2*x+1,0)] - 2; h[coo(x+1,0)] = z; }
	for (int y=0; y<m; ++y) {
		int z = h[coo(0,y)] + 4*A[coo(0,2*y)] + 4*A[coo(0,2*y+1)] - 2; h[coo(0,y+1)] = z;
		for (int x=0; x<m; ++x) { z -= 4*A[coo(2*x,2*y+1)] + 4*A[coo(2*x+1,2*y+1)] - 2; h[coo(x+1,y+1)] = z; }
	}
	return h;
}

int main (int argc, char ** argv) {
	H.init ("Domino shuffle", argc,argv, "m=50,a=1,b=.5,c=.3,s=0,r=0,w=unif");
	int m = H['m'];
	int seed = H['s']; if (seed) prng.seed (seed);

	Array<double> TP;
	if (H['w'] == "unif") TP = weights (m, Array<double> (1,1,1)); else
	if (H['w'] == "two") TP = twoperiodic (m,H['a'],H['b']); else
	if (H['w'] == "three") TP = threeperiodic (m,H['a'],H['b'],H['c']); else
	if (H['w'] == "kenyon") TP = threebytwo (m,H['b']); else {
		H.L->error (R"(No such weight, "{}".)", string(H['w'])); exit(1);
	}

	auto A1 = aztecgen(TP); auto H1 = height(A1);

	string name = H.dir + H.title; ofstream asy (name + ".asy");
	for (auto z : coos(A1)) if (A1[z]) {
		coo edge (1, (z.x+z.y)%2 ? 1 : -1);
		auto s = [](coo z){
			coo zz = (z+coo{1,1})/2;
			coo sh = dz[(zz.y + (((zz.x+1)%4)/2 ? 5 : 3)) % 4];
			return cpx(z) + 2 * double(H['r']) * cpx(sh);
		};
		auto l = [](cpx z){ return fmt::format ("({},{})", real(z), imag(z)); };
		asy << "draw (" << l(s(z*2-edge)) << "--" << l(s(z*2+edge)) << ", gray (" << TP[z]/1.3 << "));" << endl;
	}
	int L = A1.ww*2+2; asy << "draw ((-4,-4) -- ("<<L<<",-4) -- ("<<L<<","<<L<<") -- (-4,"<<L<<") -- (-4,-4), gray(1));" << endl;

	ofstream dat (name + ".dat");
	for (auto z : coos(H1)) { dat << H1[z] << " "; if (z.x == H1.ww-1) dat << endl; }

	if (system (("asy -fpdf -o " + H.dir + R"( ")" + name + R"(.asy")").c_str())) H.L->error ("Couldn't run asymptote");
}
