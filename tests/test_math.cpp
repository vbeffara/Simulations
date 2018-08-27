#include <vb/Elliptic.h>

#define bla(f)                                                                                                                             \
    {                                                                                                                                      \
        cout << #f << " " << EE.f() << E.f() << "\n\t\t\t" << norm(EE.f() - cpx(double_approx(real(E.f())), double_approx(imag(E.f()))))   \
             << endl;                                                                                                                      \
    }
#define blaz(f)                                                                                                                            \
    {                                                                                                                                      \
        cout << #f << " " << EE.f(zz) << E.f(z) << "\n\t\t\t"                                                                              \
             << norm(EE.f(zz) - cpx(double_approx(real(E.f(z))), double_approx(imag(E.f(z))))) << endl;                                    \
    }

using namespace vb;
using namespace std;

int main() {
    cln::default_float_format = cln::float_format(100);

    cpx              ttau(.125, 1.25), qq(q_<double>(ttau)), zz(.25, .75);
    Elliptic<double> EE{qq};
    complex_t        tau = to_cpx<real_t>(1, 10) / 8, q(q_<real_t>(tau)), z = to_cpx<real_t>(1, 3) / 4;
    Elliptic<real_t> E{q};

    cout << "tau = " << ttau << tau << endl;
    cout << "z   = " << zz << z << endl;
    cout << "q   = " << qq << q << endl;
    cout << "q14 = " << EE.q14 << E.q14 << endl;
    cout << "q_t = " << q_t<double>(ttau) << q_t<real_t>(tau) << endl;
    cout << endl;
    bla(g2);
    bla(g3);
    cout << endl;
    blaz(theta1);
    blaz(theta1_z);
    blaz(theta1_zz);
    blaz(theta1_zzz);
    blaz(theta1_q);
    blaz(theta1_zq);
    blaz(theta1_zzq);
    blaz(theta1_zzzq);
    cout << endl;
    blaz(theta2);
    blaz(theta2_z);
    blaz(theta2_q);
    cout << endl;
    blaz(theta3);
    blaz(theta3_q);
    cout << endl;
    blaz(theta4);
    blaz(theta4_q);
    cout << endl;
    bla(eta1);
    bla(eta1_q);
    cout << endl;
    blaz(sigma);
    blaz(sigma_q);
    cout << endl;
    blaz(zeta);
    blaz(zeta_q);
    cout << endl;
    blaz(wp);
    blaz(wp_z);
    blaz(wp_q);
    cout << endl;
}
