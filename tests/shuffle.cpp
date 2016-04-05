// Initial code in Python by Sunil Chhita, ported to C++ by VB.

#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

auto threeperiodic (int m, double a, double b, double c) {
    vector<vector<double>> A (6*m, vector<double> (6*m));
    vector<vector<double>> P {{a,b,c},{b,c,a},{c,a,b}};
    for (int i=0; i<6*m; ++i) for (int j=0; j<6*m; ++j) A[i][j] = P[(i/2)%3][(j/2)%3];
    return A;
}

auto d3p (const vector<vector<double>> & x1) {
    vector<vector<vector<vector<double>>>> A;
    int n = x1.size(); A.resize (n/2);

    A[0].resize (n, vector<vector<double>> (n));
    for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) if (x1[i][j] == 0) A[0][i][j] = {1,1}; else A[0][i][j] = {x1[i][j],0};

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

auto probs (const vector<vector<double>> & x1) {
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

auto aztecgen (vector<vector<double>> xr) {
    auto x0 = probs(xr);
    int n = x0.size();
    vector<vector<int>> a1;
    if (prng.bernoulli(x0[0][0][0])) a1 = {{1,0},{0,1}}; else a1 = {{0,1},{1,0}};
    for (int i=0; i<n-1; ++i) { a1=delslide(a1); create(a1,x0[i+1]); }
    return a1;
}

int main (int argc, char ** argv) {
    H.init ("Sunil's domino shuffle ported to C++", argc,argv, "m=8,a=1,b=.7,c=.1");
    int m = H['m'];
    auto TP = threeperiodic(m,H['a'],H['b'],H['c']);
    auto A1 = aztecgen(TP);
    for (int y=0; y<6*m; ++y) {
        for (int x=0; x<6*m; ++x) {
            if (A1[y][x]) {
                if ((x+y) % 2) cout << "\\";
                else cout << "/";
            } else cout << ".";
        }
        cout << endl;
    }
}
