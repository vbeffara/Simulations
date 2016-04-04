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

auto d3p (vector<vector<double>> x1) {
    int n = x1.size();
    vector<vector<vector<vector<double>>>> A;
    vector<vector<vector<double>>> B;

    for (int i=0; i<n; ++i) {
        vector<vector<double>> row;
        for (int j=0; j<n; ++j) {
            if (x1[i][j] == 0) row.push_back ({1,1});
            else row.push_back ({x1[i][j],0});
        }
        B.push_back(row);
    }
    A.push_back(B);

    for (int k=0; k<n/2-1; ++k) {
        vector<vector<vector<double>>> C;
        for (int i=0; i<n-2*k-2; ++i) {
            vector<vector<double>> row;
            for (int j=0; j<n-2*k-2; ++j) {
                vector<double> a1 = A[k][i+2*(i%2)][j+2*(j%2)], a2;
                if (A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] == A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1]) {
                    a2 = { A[k][i+2*(i%2)][j+2*(j%2)][0] * A[k][i+1][j+1][0] + A[k][i+2*(i%2)][j+1][0] * A[k][i+1][j+2*(j%2)][0],
                        A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] };
                } else if (A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] < A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1]) {
                    a2 = { A[k][i+2*(i%2)][j+2*(j%2)][0] * A[k][i+1][j+1][0], A[k][i+2*(i%2)][j+2*(j%2)][1] + A[k][i+1][j+1][1] };
                } else {
                    a2 = { A[k][i+2*(i%2)][j+1][0] * A[k][i+1][j+2*(j%2)][0], A[k][i+2*(i%2)][j+1][1] + A[k][i+1][j+2*(j%2)][1] };
                }
                row.push_back ( { a1[0]/a2[0], a1[1]-a2[1] } );
            }
            C.push_back(row);
        }
        A.push_back(C);
    }
    return A;
}

auto probs (vector<vector<double>> x1) {
    auto a0 = d3p (x1);
    int n = a0.size();
    vector<vector<vector<double>>> A;

    for (int k=0; k<n; ++k) {
        vector<vector<double>> C;
        for (int i=0; i<k+1; ++i) {
            vector<double> row;
            for (int j=0; j<k+1; ++j) {
                if (a0[n-k-1][2*i][2*j][1] + a0[n-k-1][2*i+1][2*j+1][1] > a0[n-k-1][2*i+1][2*j][1] + a0[n-k-1][2*i][2*j+1][1])
                    row.push_back (0);
                else if (a0[n-k-1][2*i][2*j][1] + a0[n-k-1][2*i+1][2*j+1][1] < a0[n-k-1][2*i+1][2*j][1] + a0[n-k-1][2*i][2*j+1][1])
                    row.push_back (1);
                else
                    row.push_back (a0[n-k-1][2*i+1][2*j+1][0] * a0[n-k-1][2*i][2*j][0] /
                        (a0[n-k-1][2*i+1][2*j+1][0] * a0[n-k-1][2*i][2*j][0] + a0[n-k-1][2*i+1][2*j][0]*a0[n-k-1][2*i][2*j+1][0]));
            }
            C.push_back(row);
        }
        A.push_back(C);
    }
    return A;
}

auto delslide (vector<vector<int>> x1) {
    int n = x1.size();
    vector<vector<int>> a0;
    for (int i=0; i<n+2; ++i) {
        vector<int> row;
        for (int j=0; j<n+2; ++j) {
            if ((i==0) || (i==n+1) || (j==0) || (j==n+1))
                row.push_back (0);
            else
                row.push_back (x1[i-1][j-1]);
        }
        a0.push_back (row);
    }
    for (int i=0; i<n/2; ++i) {
        for (int j=0; j<n/2; ++j) {
            if ((a0[2*i][2*j]==1) && (a0[2*i+1][2*j+1]==1)) {
                a0[2*i][2*j]=0;
                a0[2*i+1][2*j+1]=0;
            }
            else if ((a0[2*i][2*j+1]==1) && (a0[2*i+1][2*j]==1)) {
                a0[2*i+1][2*j]=0;
                a0[2*i][2*j+1]=0;
            }
        }
    }
    for (int i=0; i<n/2+1; ++i) {
        for (int j=0; j<n/2+1; ++j) {
            if (a0[2*i+1][2*j+1]==1) {
                a0[2*i][2*j]=1;
                a0[2*i+1][2*j+1]=0;
            } else if (a0[2*i][2*j]==1) {
                a0[2*i][2*j]=0;
                a0[2*i+1][2*j+1]=1;
            } else if (a0[2*i+1][2*j]==1) {
                a0[2*i][2*j+1]=1;
                a0[2*i+1][2*j]=0;
            } else if (a0[2*i][2*j+1]==1) {
                a0[2*i+1][2*j]=1;
                a0[2*i][2*j+1]=0;
            }
        }
    }
    return a0;
}

auto create (vector<vector<int>> x0, vector<vector<double>> p) {
    int n = x0.size();
    for (int i=0; i<n/2; ++i) {
        for (int j=0; j<n/2; ++j) {
            if ((x0[2*i][2*j]==0) && (x0[2*i+1][2*j]==0) && (x0[2*i][2*j+1]==0) && (x0[2*i+1][2*j+1]==0)) {
                bool a1,a2,a3,a4;
                if (j>0)
                    a1 = (x0[2*i][2*j-1]==0) && (x0[2*i+1][2*j-1]==0);
                else
                    a1 = true;
                if (j<n/2-1)
                    a2 = (x0[2*i][2*j+2]==0) && (x0[2*i+1][2*j+2]==0);
                else
                    a2 = true;
                if (i>0)
                    a3 = (x0[2*i-1][2*j]==0) && (x0[2*i-1][2*j+1]==0);
                else
                    a3 = true;
                if (i<n/2-1)
                    a4 = (x0[2*i+2][2*j]==0) && (x0[2*i+2][2*j+1]==0);
                else
                    a4 = true;
                if (a1 && a2 && a3 && a4) {
                    if (prng.bernoulli(p[i][j])) {
                        x0[2*i][2*j]=1;
                        x0[2*i+1][2*j+1]=1;
                    } else {
                        x0[2*i+1][2*j]=1;
                        x0[2*i][2*j+1]=1;
                    }
                }
            }
        }
    }
    return x0;
}

auto aztecgen (vector<vector<double>> xr) {
    auto x0 = probs(xr);
    int n = x0.size();
    vector<vector<int>> a1;
    if (prng.bernoulli(x0[0][0][0]))
        a1 = {{1,0},{0,1}};
    else
        a1 = {{0,1},{1,0}};
    for (int i=0; i<n-1; ++i) {
        a1=delslide(a1);
        a1=create(a1,x0[i+1]);
    }
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
