#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vb/util/cpx.h>

using namespace vb;

struct mobius {
    double a;
    double theta;

    mobius(double _a, double _theta) : a(_a), theta(_theta) {}
    mobius(cpx zm, cpx zp) {
        if (abs(zp / zm + 1.0) < .001) {
            a     = 0;
            theta = arg(sqrt(zp * zm));
        } else {
            cpx w = sqrt(zp / zm);
            a     = real((w - I) / (1.0 - w * I));
            theta = arg(zm * w);
        }
    }

    cpx    operator()(cpx z) { return exp(I * theta) * (a + z) / (1.0 + a * z); }
    mobius operator()(mobius m) { return {(*this)(m(-I)), (*this)(m(I))}; }

    std::string repr() const { return fmt::format("[a={}, theta={}]", a, theta * 180 / M_PI); }

    std::string draw() {
        std::string r = repr();
        if (a > .001)
            return fmt::format("{} circle ({}) % {}", exp(I * theta) * (1 + a * a) / (2 * a), (1 - a * a) / (2 * a), r);
        else {
            double th = theta * 180 / M_PI;
            return fmt::format("[rotate={}] (0,-1) rectangle (1,2) [rotate={}] % {}", th, -th, r);
        }
    }
};

int main() {
    mobius gen1(0, 0), gen2(0, -M_PI / 6), gen3(sqrt(sqrt(5.0) - 2), M_PI / 3);
    std::cout << gen1.draw() << std::endl;
    std::cout << gen2.draw() << std::endl;
    std::cout << gen3.draw() << std::endl;
}
