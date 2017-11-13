#include <vb/LinearAlgebra.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

vector<Matrix<double>> rho,Gamma;

double lambda (const Matrix<double> & A) {
    return abs (A(0,0) + A(1,1));
}

double n2 (int n) {
    Matrix<double> A (2,2), B (2,2); A(0,0)=1; A(0,1)=0; A(1,0)=0; A(1,1)=1; B=A;
    for (int i=0; i<n; ++i) {
        auto d = prng.uniform_int(4);
        A *= rho[d];
        B *= Gamma[d];
    }
    return log(lambda(A)) / log(lambda(B));
}

int main (int argc, char ** argv) {
    H.init ("Erwan's product of random matrices", argc,argv, "n=100,t=100");

    Matrix<double> A (2,2);

    A(0,0)=1; A(0,1)=sqrt(3.0); A(1,0)=0; A(1,1)=1; rho.push_back(A);
    A(0,0)=1; A(0,1)=-sqrt(3.0); A(1,0)=0; A(1,1)=1; rho.push_back(A);
    A(0,0)=1; A(0,1)=0; A(1,0)=sqrt(3.0); A(1,1)=1; rho.push_back(A);
    A(0,0)=1; A(0,1)=0; A(1,0)=-sqrt(3.0); A(1,1)=1; rho.push_back(A);

    A(0,0)=1; A(0,1)=2; A(1,0)=0; A(1,1)=1; Gamma.push_back(A);
    A(0,0)=1; A(0,1)=-2; A(1,0)=0; A(1,1)=1; Gamma.push_back(A);
    A(0,0)=1; A(0,1)=0; A(1,0)=2; A(1,1)=1; Gamma.push_back(A);
    A(0,0)=1; A(0,1)=0; A(1,0)=-2; A(1,1)=1; Gamma.push_back(A);

    vector<double> n2s;
    for (int i=0; i<int(H['t']); ++i) n2s.push_back(n2(H['n']));
    sort(begin(n2s),end(n2s));
    for (auto v : n2s) cout << v << endl;
}