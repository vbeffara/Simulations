#include <spdlog/spdlog.h>
#include <vb/Elliptic.h>
#include <vb/util/Hub.h>
#include <vb/util/mp.h>

using namespace vb;
using namespace std;

void out2(const std::string &s, cpx z, const complex_t &zz) {
    spdlog::info("{} : {} and {}", s, z, zz);
    spdlog::info("  {}", real(zz));
    spdlog::info("  {}", imag(zz));
}

int main(int argc, char **argv) {
    Hub H("Tests for elliptic functions", argc, argv, "n=100");
    real_t::default_precision(H['n']);
    complex_t::default_precision(H['n']);

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
    out2("q_t", q_t<double>(ttau), q_t<real_t>(tau));

    out2("Eta1", EE.eta1(), E.eta1());
    out2("Eta1_q", EE.eta1_q(), E.eta1_q());
    out2("E1", EE.e1(), E.e1());
    out2("E1_q", EE.e1_q(), E.e1_q());
    out2("g2", EE.g2(), E.g2());
    out2("g3", EE.g3(), E.g3());
    out2("J", EE.j(), E.j());

    out2("Theta1(z)", EE.theta1(zz), E.theta1(z));
    out2("Theta1_z(z)", EE.theta1_z(zz), E.theta1_z(z));
    out2("Theta1_zz(z)", EE.theta1_zz(zz), E.theta1_zz(z));
    out2("Theta1_zzz(z)", EE.theta1_zzz(zz), E.theta1_zzz(z));
    out2("Theta1_q(z)", EE.theta1_q(zz), E.theta1_q(z));
    out2("Theta1_zq(z)", EE.theta1_zq(zz), E.theta1_zq(z));
    out2("Theta1_zzq(z)", EE.theta1_zzq(zz), E.theta1_zzq(z));
    out2("Theta1_zzzq(z)", EE.theta1_zzzq(zz), E.theta1_zzzq(z));

    out2("Theta2(z)", EE.theta2(zz), E.theta2(z));
    out2("Theta2_z(z)", EE.theta2_z(zz), E.theta2_z(z));
    out2("Theta2_q(z)", EE.theta2_q(zz), E.theta2_q(z));
    out2("Theta3(z)", EE.theta3(zz), E.theta3(z));
    out2("Theta3_q(z)", EE.theta3_q(zz), E.theta3_q(z));
    out2("Theta4(z)", EE.theta4(zz), E.theta4(z));
    out2("Theta4_q(z)", EE.theta4_q(zz), E.theta4_q(z));

    out2("Sigma(z)", EE.sigma(zz), E.sigma(z));
    out2("Sigma_q(z)", EE.sigma_q(zz), E.sigma_q(z));
    out2("Zeta(z)", EE.zeta(zz), E.zeta(z));
    out2("Zeta_q(z)", EE.zeta_q(zz), E.zeta_q(z));

    out2("WP", EE.wp(zz), E.wp(z));
    out2("WP_z", EE.wp_z(zz), E.wp_z(z));
    out2("WP_q", EE.wp_q(zz), E.wp_q(z));
}
