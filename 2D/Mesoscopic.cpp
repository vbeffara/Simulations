#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/Toroidal.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
	Hub H ("Toroidal triangulation",argc,argv,"n=4,o=0,m=4,f,g=C5");
	HLib HL; Toroidal G (HL.at(H['g']),H);

	for (int i=H['o']; i<=int(H['n']); ++i) {
		cerr << "Step " << i << ": " << G;
		G.pack(); if(H['f']) G.flip();

		cpx q = exp(I * M_PI * G.m);
		cpx theta2(0), old_theta2(-1); for (int n=0; theta2 != old_theta2; ++n) { old_theta2=theta2; theta2 += 2.0 * pow (q, (n+.5)*(n+.5)); }
		cpx theta3(1), old_theta3(-1); for (int n=1; theta3 != old_theta3; ++n) { old_theta3=theta3; theta3 += 2.0 * pow (q, n*n); }
		cpx lambda = pow(theta2/theta3,4);
		cpx j = 256.0 * pow(1.0-lambda+pow(lambda,2),3) / pow(lambda*(1.0-lambda),2);

		cout << "  tau = " << G.m << endl;
		cout << "  j   = " << j << endl;

		if (i<int(H['n'])) { G.split_edges(); G.from_hypermap(); }
	}

	G.output_pdf();
}
