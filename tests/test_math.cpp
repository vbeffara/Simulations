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
	cout << "g2 = " << EE.g2() << E.g2() << endl;
	cout << "g3 = " << EE.g3() << E.g3() << endl;
	cout << endl;
	cout << "theta1      = " << EE.theta1(zz) << E.theta1(z) << endl;
	cout << "theta1_z    = " << EE.theta1_z(zz) << E.theta1_z(z) << endl;
	cout << "theta1_zz   = " << EE.theta1_zz(zz) << E.theta1_zz(z) << endl;
	cout << "theta1_zzz  = " << EE.theta1_zzz(zz) << E.theta1_zzz(z) << endl;
	cout << "theta1_q    = " << EE.theta1_q(zz) << E.theta1_q(z) << endl;
	cout << "theta1_zq   = " << EE.theta1_zq(zz) << E.theta1_zq(z) << endl;
	cout << "theta1_zzq  = " << EE.theta1_zzq(zz) << E.theta1_zzq(z) << endl;
	cout << "theta1_zzzq = " << EE.theta1_zzzq(zz) << E.theta1_zzzq(z) << endl;
	cout << endl;
	cout << "theta2   = " << EE.theta2(zz) << E.theta2(z) << endl;
	cout << "theta2_z = " << EE.theta2_z(zz) << E.theta2_z(z) << endl;
	cout << "theta2_q = " << EE.theta2_q(zz) << E.theta2_q(z) << endl;
	cout << endl;
	cout << "theta3   = " << EE.theta3(zz) << E.theta3(z) << endl;
	// cout << "theta3_z = " << EE.theta3_z(z) << endl;
	cout << "theta3_q = " << EE.theta3_q(zz) << E.theta3_q(z) << endl;
	cout << endl;
	cout << "theta4   = " << EE.theta4(zz) << E.theta4(z) << endl;
	// cout << "theta4_z = " << EE.theta3_z(z) << endl;
	cout << "theta4_q = " << EE.theta4_q(zz) << E.theta4_q(z) << endl;
	cout << endl;
	cout << "eta1   = " << EE.eta1() << E.eta1() << endl;
	cout << "eta1_q = " << EE.eta1_q() << E.eta1_q() << endl;
	cout << endl;
	cout << "sigma   = " << EE.sigma(zz) << E.sigma(z) << endl;
	cout << "sigma_q = " << EE.sigma_q(zz) << E.sigma_q(z) << endl;
	cout << endl;
	cout << "zeta   = " << EE.zeta(zz) << E.zeta(z) << endl;
	cout << "zeta_q = " << EE.zeta_q(zz) << E.zeta_q(z) << endl;
	cout << endl;
	cout << "wp   = " << EE.wp(zz) << E.wp(z) << endl;
	cout << "wp_z = " << EE.wp_z(zz) << E.wp_z(z) << endl;
	cout << "wp_q = " << EE.wp_q(zz) << E.wp_q(z) << endl;
	cout << endl;
}
