#include <vb/math.h>

using namespace vb; using namespace std;

int main (int argc, char ** argv) {
	cpx ttau(.125,1.25), qq(q_(ttau)), zz(.25,.75);
	cpx100 tau(.125,1.25), q(q_(tau)), z(.25,.75);

	cout << setprecision(40);
	cout << "tau = " << ttau << tau << endl;
	cout << "z   = " << zz << z << endl;
	cout << "q   = " << qq << q << endl;
	cout << "q_t = " << q_t(ttau) << q_t(tau) << endl;
	cout << endl;
	cout << "g2 = " << g2_(qq) << g2_(q) << endl;
	cout << "g3 = " << g3_(qq) << g3_(q) << endl;
	cout << endl;
	cout << "theta1      = " << theta1_(zz,qq) << theta1_(z,q) << endl;
	cout << "theta1_z    = " << theta1_z(zz,qq) << theta1_z(z,q) << endl;
	cout << "theta1_zz   = " << theta1_zz(zz,qq) << theta1_zz(z,q) << endl;
	cout << "theta1_zzz  = " << theta1_zzz(zz,qq) << theta1_zzz(z,q) << endl;
	cout << "theta1_q    = " << theta1_q(zz,qq) << theta1_q(z,q) << endl;
	cout << "theta1_zq   = " << theta1_zq(zz,qq) << theta1_zq(z,q) << endl;
	cout << "theta1_zzq  = " << theta1_zzq(zz,qq) << theta1_zzq(z,q) << endl;
	cout << "theta1_zzzq = " << theta1_zzzq(zz,qq) << theta1_zzzq(z,q) << endl;
	cout << endl;
	cout << "theta2   = " << theta2_(zz,qq) << theta2_(z,q) << endl;
	cout << "theta2_z = " << theta2_z(zz,qq) << theta2_z(z,q) << endl;
	cout << "theta2_q = " << theta2_q(zz,qq) << theta2_q(z,q) << endl;
	cout << endl;
	cout << "theta3   = " << theta3_(zz,qq) << theta3_(z,q) << endl;
	// cout << "theta3_z = " << theta3_z(z,q) << endl;
	cout << "theta3_q = " << theta3_q(zz,qq) << theta3_q(z,q) << endl;
	cout << endl;
	cout << "theta4   = " << theta4_(zz,qq) << theta4_(z,q) << endl;
	// cout << "theta4_z = " << theta3_z(z,q) << endl;
	cout << "theta4_q = " << theta4_q(zz,qq) << theta4_q(z,q) << endl;
	cout << endl;
	cout << "eta1   = " << eta1_(qq) << eta1_(q) << endl;
	cout << "eta1_q = " << eta1_q(qq) << eta1_q(q) << endl;
	cout << endl;
	cout << "sigma   = " << sigma_(zz,qq) << sigma_(z,q) << endl;
	cout << "sigma_q = " << sigma_q(zz,qq) << sigma_q(z,q) << endl;
	cout << endl;
	cout << "zeta   = " << zeta_(zz,qq) << zeta_(z,q) << endl;
	cout << "zeta_q = " << zeta_q(zz,qq) << zeta_q(z,q) << endl;
	cout << endl;
	cout << "wp   = " << wp_(zz,qq) << wp_(z,q) << endl;
	cout << "wp_z = " << wp_z(zz,qq) << wp_z(z,q) << endl;
	cout << "wp_q = " << wp_q(zz,qq) << wp_q(z,q) << endl;
	cout << endl;
}
