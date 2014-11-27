#include <vb/Elliptic.h>
#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	cpx   	ttau(.125,1.25),	qq(q_(ttau)),	zz(.25,.75);	Elliptic<double> EE { qq };
	cpx100	tau(.125,1.25), 	q(q_(tau)),  	z(.25,.75); 	Elliptic<gmp100> E { q };

	cout << setprecision(40);
	cout << "tau = " << ttau << tau << endl;
	cout << "z   = " << zz << z << endl;
	cout << "q   = " << qq << q << endl;
	cout << "q_t = " << q_t(ttau) << q_t(tau) << endl;
	cout << endl;
	cout << "g2 = " << EE.g2() << g2_(q) << endl;
	cout << "g3 = " << EE.g3() << g3_(q) << endl;
	cout << endl;
	cout << "theta1      = " << EE.theta1(zz) << theta1_(z,q) << endl;
	cout << "theta1_z    = " << EE.theta1_z(zz) << theta1_z(z,q) << endl;
	cout << "theta1_zz   = " << EE.theta1_zz(zz) << theta1_zz(z,q) << endl;
	cout << "theta1_zzz  = " << EE.theta1_zzz(zz) << theta1_zzz(z,q) << endl;
	cout << "theta1_q    = " << EE.theta1_q(zz) << theta1_q(z,q) << endl;
	cout << "theta1_zq   = " << EE.theta1_zq(zz) << theta1_zq(z,q) << endl;
	cout << "theta1_zzq  = " << EE.theta1_zzq(zz) << theta1_zzq(z,q) << endl;
	cout << "theta1_zzzq = " << EE.theta1_zzzq(zz) << theta1_zzzq(z,q) << endl;
	cout << endl;
	cout << "theta2   = " << EE.theta2(zz) << theta2_(z,q) << endl;
	cout << "theta2_z = " << EE.theta2_z(zz) << theta2_z(z,q) << endl;
	cout << "theta2_q = " << EE.theta2_q(zz) << theta2_q(z,q) << endl;
	cout << endl;
	cout << "theta3   = " << EE.theta3(zz) << theta3_(z,q) << endl;
	// cout << "theta3_z = " << EE.theta3_z(z,q) << endl;
	cout << "theta3_q = " << EE.theta3_q(zz) << theta3_q(z,q) << endl;
	cout << endl;
	cout << "theta4   = " << EE.theta4(zz) << theta4_(z,q) << endl;
	// cout << "theta4_z = " << EE.theta3_z(z,q) << endl;
	cout << "theta4_q = " << EE.theta4_q(zz) << theta4_q(z,q) << endl;
	cout << endl;
	cout << "eta1   = " << EE.eta1() << eta1_(q) << endl;
	cout << "eta1_q = " << EE.eta1_q() << eta1_q(q) << endl;
	cout << endl;
	cout << "sigma   = " << EE.sigma(zz) << sigma_(z,q) << endl;
	cout << "sigma_q = " << EE.sigma_q(zz) << sigma_q(z,q) << endl;
	cout << endl;
	cout << "zeta   = " << EE.zeta(zz) << zeta_(z,q) << endl;
	cout << "zeta_q = " << EE.zeta_q(zz) << zeta_q(z,q) << endl;
	cout << endl;
	cout << "wp   = " << EE.wp(zz) << wp_(z,q) << endl;
	cout << "wp_z = " << EE.wp_z(zz) << wp_z(z,q) << endl;
	cout << "wp_q = " << EE.wp_q(zz) << wp_q(z,q) << endl;
	cout << endl;
}
