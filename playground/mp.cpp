#include <vb/Elliptic.h>
#include <vb/mp.h>

#define bla(f)                                                                                                                             \
    { cout << #f << " " << EE.f() << E.f() << "\n\t\t\t" << norm(EE.f() - cpx(double(real(E.f())), double(imag(E.f())))) << endl; }
#define blaz(f)                                                                                                                            \
    { cout << #f << " " << EE.f(zz) << E.f(z) << "\n\t\t\t" << norm(EE.f(zz) - cpx(double(real(E.f(z))), double(imag(E.f(z))))) << endl; }

using namespace vb;
using namespace std;

void out(real_t x) { std::cerr << setprecision(x.backend().precision()) << fixed << x << "[" << x.backend().precision() << "]\n"; }

void out2(std::string s, cpx z, complex_t zz) {
    H.L->info("{} : {} and {}", s, z, zz);
    out(real(zz));
    out(imag(zz));
}

int main() {
    real_t::default_precision(150);
    complex_t::default_precision(150);

    cpx       ttau(.125, 1.25);
    complex_t tau = complex_t(1, 10) / 8;
    out2("Tau", ttau, tau);

    cpx       qq(q_<double>(ttau));
    complex_t q(q_<real_t>(tau));
    out2("q", qq, q);

    cpx       zz(.25, .75);
    complex_t z = complex_t(1, 3) / 4;
    out2("Z", zz, z);

    Elliptic<double> EE{qq};
    Elliptic<real_t> E{q};

    out2("q14", EE.q14, E.q14);

    out2("Eta1", EE.eta1(), E.eta1());
    out2("Eta1_q", EE.eta1_q(), E.eta1_q());
    out2("E1", EE.e1(), E.e1());
    out2("E1_q", EE.e1_q(), E.e1_q());
    out2("g2", EE.g2(), E.g2());
    out2("g3", EE.g3(), E.g3());
    out2("J", EE.j(), E.j());

    out2("Theta1(z)", EE.theta1(zz), E.theta1(z));
    out2("Theta2(z)", EE.theta2(zz), E.theta2(z));
    out2("Theta3(z)", EE.theta3(zz), E.theta3(z));
    out2("Theta4(z)", EE.theta4(zz), E.theta4(z));

    out2("Sigma(z)", EE.sigma(zz), E.sigma(z));

    out2("Zeta(z)", EE.zeta(zz), E.zeta(z));

    out2("WP", EE.wp(zz), E.wp(z));

    /*
                    cplx theta1_z   	(const cplx & z) const;
                    cplx theta1_zz  	(const cplx & z) const;
                    cplx theta1_zzz 	(const cplx & z) const;
                    cplx theta1_q   	(const cplx & z) const;
                    cplx theta1_zq  	(const cplx & z) const;
                    cplx theta1_zzq 	(const cplx & z) const;
                    cplx theta1_zzzq	(const cplx & z) const;
                    cplx theta2_z   	(const cplx & z) const;
                    cplx theta2_q   	(const cplx & z) const;
                    cplx theta3_q   	(const cplx & z) const;
                    cplx theta4_q   	(const cplx & z) const;

                    cplx sigma_q	(const cplx & z) const;
                    cplx zeta_q 	(const cplx & z) const;
                    cplx wp     	(const cplx & z) const;
                    cplx wp_z   	(const cplx & z) const;
                    cplx wp_q   	(const cplx & z) const;

                    cplx q;

                    cplx sum (std::function <cplx(int)>) const;

                    cplx q14, eta1_, eta1_q_, e1_;
            };
    }

    */

    // cout << "q_t = " << q_t<double>(ttau) << q_t<real_t>(tau) << endl;
    // bla(g2);
    // bla(g3);
    // blaz(theta4);
    // bla(eta1);
    // blaz(sigma);
    // blaz(zeta);
    // blaz(wp);
}
