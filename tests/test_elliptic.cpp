#include <vb/Constellation1.h>
#include <vb/Hub.h>
#include <vb/Hypermap_lib.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	cpx pole {0.5,sqrt(3.0)/6}, tau {.5,sqrt(3.0)/2};
	// vector<cpx> zeros { 0,0,0,0,0,0 };
	// vector<cpx> poles { pole, pole, pole, -pole, -pole, -pole };
	// vector<cpx> ones { .5, cpx(.25,sqrt(3.0)/4), cpx(.75,sqrt(3.0)/4) };

	// cpx q = q_(tau);
	// cpx theta1 = theta1_(0.1,q);
	// cpx theta1prime = theta1prime_(0,q);
	// cpx theta2 = theta2_(0.1,q);
	// cpx theta3 = theta3_(0.1,q);
	// cpx theta4 = theta4_(0.1,q);
	// cpx lambda = pow(theta2/theta3,4);
	// cpx j = 256.0 * pow(1.0-lambda+pow(lambda,2),3) / pow(lambda*(1.0-lambda),2);

	// cout << "    tau: " << tau << endl;
	// cout << "      q: " << q << endl;
	// cout << " theta1: " << theta1 << endl;
	// cout << "theta'1: " << theta1prime << endl;
	// cout << "theta''1: " << theta1second_(0,q) << endl;
	// cout << "theta'''1: " << theta1third_(0,q) << endl;
	// cout << " theta2: " << theta2 << endl;
	// cout << " theta3: " << theta3 << endl;
	// cout << " theta4: " << theta4 << endl;
	// cout << " lambda: " << lambda << endl;
	// cout << "      j: " << j << endl;

	// cout << "eta1(q) = " << eta1_(q) << endl;
	// cout << "g2(q) = " << g2_(q) << endl;
	// cout << "g3(q) = " << g3_(q) << endl;
	// cout << "exp(eta1*.1^2) = " << exp(eta1_(q)*.1*.1) << endl;
	// cout << "sigma(.1) = " << sigma_(.1,q) << endl;

	// cout << "Zeros:" << endl; for (auto z : zeros) cout << "  " << z << endl;
	// cout << "Poles:" << endl; for (auto z : poles) cout << "  " << z << endl;
	// cout << "Ones:" << endl; for (auto z : ones) cout << "  " << z << endl;

	// cpx z {.1,.1}, sz {1,0};
	// for (auto w : zeros) sz *= sigma_(z-w,q);
	// for (auto w : poles) sz /= sigma_(z-w,q);
	// cout << "wp(.1+.1I,(1+sqrt(3)I)/2) = " << wp_(cpx(.1,.1),q) << endl;
	// cout << "zeta(.1+.1I,(1+sqrt(3)I)/2) = " << zeta_(cpx(.1,.1),q) << endl;

	Hub H ("Testing 1",argc,argv,"n=3,m=228,v,q,g=lat_csquare,p");
	Hypermap M = HLib().at(H['g']); cout << M << endl;
	Constellation1<double> C {M,H,H['n']}; C.findn();
	cout << C << endl;
	if (H['v']) { Image * img = C.draw(800); img->show(); img->pause(); }
}
