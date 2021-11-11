#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vb/util/cpx.h>

using namespace vb;

struct mobius {
    cpx    a;
    double theta;

    mobius(cpx _a, double _theta) : a(_a), theta(_theta) {}
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

    cpx    operator()(cpx z) const { return exp(I * theta) * (a + z) / (1.0 + conj(a) * z); }
    mobius operator()(mobius m) const { return {(*this)(m(-I)), (*this)(m(I))}; }
    mobius inverse() const { return {-a * exp(I * theta), -theta}; }
    cpx    sym(cpx z) const { return (*this)(-conj(inverse()(z))); }
    mobius sym(mobius m) const { return {sym(m(I)), sym(m(-I))}; }

    bool like(const mobius &m) const {
        if ((abs(a) < .001) && (abs(m.a) < .001) && (abs(abs(theta - m.theta) - M_PI) < .001)) return true;
        if (abs(a - m.a) > .001) return false;
        if (abs(theta - m.theta) < .001) return true;
        return (abs(abs(theta - m.theta) - 2 * M_PI) < .001);
    }

    std::string repr() const { return fmt::format("[a={}, theta={}]", a, theta * 180 / M_PI); }

    std::string draw() const {
        std::string r = repr();
        if (abs(a) > .001) {
            cpx    center = exp(I * theta) * (1.0 + a * a) / (a + conj(a));
            double radius = real((1.0 - a * conj(a)) / (a + conj(a)));
            return fmt::format("{} circle ({}) % {}", center, radius, r);
        } else {
            double th = theta * 180 / M_PI;
            return fmt::format("[rotate={}] (0,-1) rectangle (1,2) [rotate={}] % {}", th, -th, r);
        }
    }
};

int main() {
    mobius              gen1(0, 0), gen2(0, -M_PI / 6), gen3(sqrt(sqrt(5.0) - 2), M_PI / 3);
    std::vector<mobius> list{gen1, gen2, gen3};
    bool                done = false;
    while (!done) {
        std::vector<mobius> candidates;
        for (const auto &m : list) {
            for (const auto &n : list) { candidates.push_back(m.sym(n)); }
        }
        done = true;
        for (const auto &m : candidates) {
            if (abs(m.a) > .999) continue;
            if (std::none_of(begin(list), end(list), [&](const mobius &n) { return m.like(n); })) {
                list.push_back(m);
                done = false;
            }
        }
    }
    for (const auto &m : list) std::cout << m.draw() << std::endl;
}
