#include <fmt/ostream.h>
#include <iostream>
#include <spdlog/spdlog.h>
#include <vb/util/cpx.h>

using namespace vb;

struct mobius {
    cpx    a;
    double theta;

    static auto from_endpoints(cpx zm, cpx zp) -> mobius {
        if (norm(zp / zm + 1.0) < .000001) return {0, arg(zp * zm) / 2};
        cpx const w = sqrt(zp / zm);
        return {real((w - I) / (1.0 - w * I)), arg(zm * w)};
    }

    [[nodiscard]] auto operator()(cpx z) const -> cpx { return exp(I * theta) * (a + z) / (1.0 + conj(a) * z); }
    [[nodiscard]] auto inverse() const -> mobius { return {-a * exp(I * theta), -theta}; }
    [[nodiscard]] auto sym(cpx z) const -> cpx { return (*this)(-conj(inverse()(z))); }
    [[nodiscard]] auto sym(mobius m) const -> mobius { return from_endpoints(sym(m(I)), sym(m(-I))); }

    [[nodiscard]] auto like(const mobius &m) const -> bool {
        if ((norm(a) < .001) && (norm(m.a) < .001) && (abs(abs(theta - m.theta) - M_PI) < .001)) return true;
        if (norm(a - m.a) > .000001) return false;
        if (abs(theta - m.theta) < .001) return true;
        return (abs(abs(theta - m.theta) - 2 * M_PI) < .001);
    }

    [[nodiscard]] auto repr() const -> std::string { return fmt::format("[a={}, theta={}]", a, theta * 180 / M_PI); }

    [[nodiscard]] auto draw() const -> std::string {
        if (norm(a) > .001) {
            cpx    center = exp(I * theta) * (1.0 + a * a) / (2 * real(a));
            double radius = (1.0 - norm(a)) / (2 * real(a));
            return fmt::format("{} circle ({}) % {}", center, radius, repr());
        }
        double th = theta * 180 / M_PI;
        return fmt::format("[rotate={}] (0,-1) rectangle (1,2) [rotate={}] % {}", th, -th, repr());
    }
};

auto main() -> int {
    std::vector<mobius> gens{{0, 0}, {0, -M_PI / 6}, {sqrt(sqrt(5.0) - 2), M_PI / 3}}, list{gens};
    bool                done = false;
    while (!done) {
        std::vector<mobius> candidates;
        for (const auto &m : gens)
            for (const auto &n : list) candidates.push_back(m.sym(n));
        done = true;
        for (const auto &m : candidates) {
            if (norm(m.a) > .98) continue;
            if (std::any_of(begin(list), end(list), [&m](const mobius &n) { return m.like(n); })) continue;
            list.push_back(m);
            done = false;
        }
    }
    for (const auto &m : list) std::cout << m.draw() << std::endl;
}
