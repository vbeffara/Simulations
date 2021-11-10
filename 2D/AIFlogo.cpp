#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vb/util/cpx.h>

using namespace vb;

struct mobius {
    double a;
    cpx    eitheta;

    mobius(double _a, double _theta) : mobius(_a, exp(I * _theta)) {}
    mobius(double _a, cpx _eitheta) : a(_a), eitheta(_eitheta) {}
    mobius(cpx zm, cpx zp) {
        if (abs(zp / zm + 1.0) < .001) {
            a       = 0;
            eitheta = sqrt(zp * zm);
        } else {
            cpx w   = sqrt(zp / zm);
            a       = real((w - I) / (1.0 - w * I));
            eitheta = zm * w;
        }
    }

    cpx operator()(cpx z) { return eitheta * (a + z) / (1.0 + a * z); }

    mobius operator()(mobius m) {
        cpx zm = m(-I), zp = m(I);
        cpx fzm = (*this)(zm), fzp = (*this)(zp);
        return {fzm, fzp};
    }

    std::string repr() const { return fmt::format("[a={}, eitheta={}]", a, eitheta); }

    std::string draw() {
        std::string r = repr();
        if (a > .001)
            return fmt::format("{} circle ({}) % {}", eitheta * (1 + a * a) / (2 * a), (1 - a * a) / (2 * a), r);
        else {
            double theta = arg(eitheta) * 180 / M_PI;
            return fmt::format("[rotate={}] (0,-1) rectangle (1,2) [rotate={}]", theta, -theta);
        }
    }
};

int main() {
    mobius gen1(0, 0), gen2(0, -M_PI / 6), gen3(sqrt(sqrt(5.0) - 2), M_PI / 3);
    std::cout << gen1.draw() << std::endl;
    std::cout << gen2.draw() << std::endl;
    std::cout << gen3.draw() << std::endl;
}
