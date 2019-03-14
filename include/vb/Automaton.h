#pragma once /// \file
#include <vb/Bitmap.h>
#include <vb/util/PRNG.h>

namespace vb {
    template <class T> class Automaton;

    template <class T> class Rule {
    public:
        using Pattern = std::vector<std::pair<int, T>>;

        Rule(Pattern in, Pattern out) : cond(std::move(in)), move(std::move(out)) {}

        bool check(Automaton<T> &A, size_t i) {
            for (auto &c : cond)
                if (!(A.atp(int(i) + c.first) == c.second)) return false;
            return true;
        }
        void trigger(Automaton<T> &A, size_t i) {
            if (check(A, i))
                for (auto &m : move) A.atp(int(i) + m.first) = m.second;
        }

    private:
        Pattern cond, move;
    };

    template <class T> class Automaton : public std::vector<T> {
    public:
        Automaton(const std::string &title, size_t n_, T s, bool p = true) : std::vector<T>(n_, s), n(n_), pause(p), I(title, {n, 500}) {}

        T &atp(int i) {
            int j = i % int(n);
            if (j < 0) j += n;
            return (*this)[size_t(j)];
        }

        void show() { I.show(); }

        void step() {
            // TODO: is that reimplementing prng::discrete ?
            auto   U = prng.uniform_real(0, lt);
            size_t j = 0;
            while (U > rates[j]) U -= rates[j++];
            rules[j].trigger(*this, prng.uniform_int(n));
        }

        void swipe() {
            for (size_t i = 0; i < n * lt; ++i) step();
            for (size_t i = 0; i < n; ++i) I.put({i, y}, (*this)[i]);
            ++y;
            if (y == 500) {
                y = 0;
                if (pause) I.pause();
            }
        }

        void add_rule(double r, Rule<T> R) {
            lt += r;
            rates.push_back(r);
            rules.push_back(R);
        }

    private:
        std::vector<double>  rates;
        std::vector<Rule<T>> rules;
        size_t               n;
        size_t               y{0};
        double               lt{0};
        bool                 pause;
        Bitmap<T>            I;
    };
} // namespace vb
