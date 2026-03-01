#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

constexpr unsigned MAIN_BIT = 1U;
constexpr unsigned L1_BIT   = 2U;
constexpr unsigned L2_BIT   = 4U;
constexpr unsigned R1_BIT   = 8U;
constexpr unsigned R2_BIT   = 16U;
constexpr unsigned ALL_BITS = 31U;

class Automaton {
public:
    explicit Automaton(size_t n);

    void swap() { main.swap(alt); };

    void randomize(double /*e*/);
    void shift();
    void emit();
    void forget(double r);
    void effect(double r);

    size_t               size;
    std::vector<uint8_t> main;
    std::vector<uint8_t> alt;
};

Automaton::Automaton(size_t n) : size(n), main(n, 0), alt(n) {}

void Automaton::randomize(double e) {
    for (size_t i = 0; i < size; ++i)
        if (vb::prng.bernoulli(e)) main[i] = vb::prng() & ALL_BITS;
}

void Automaton::shift() {
    for (size_t i = 0; i < size; ++i)
        alt[i] = uint8_t((main[i] & MAIN_BIT) + (main[(i + size - 1) % size] & R1_BIT) + (main[(i + size - 1) % size] & R2_BIT) +
                         (main[(i + 1) % size] & L1_BIT) + (main[(i + 1) % size] & L2_BIT));
    this->swap();
}

void Automaton::emit() {
    for (size_t i = 0; i < size; i++) {
        alt[i] = main[i];
        if (((main[i] & 1U) != 0) && ((main[(i + size - 1) % size] & 1U) == 0)) alt[i] |= L1_BIT | R1_BIT;
        if (((main[i] & 1U) == 0) && ((main[(i + size - 1) % size] & 1U) != 0)) alt[i] |= L2_BIT | R2_BIT;
    }
    this->swap();
}

void Automaton::forget(double r) {
    for (size_t i = 0; i < size; ++i)
        if (vb::prng.bernoulli(r)) main[i] &= 1U;
}

void Automaton::effect(double r) {
    for (size_t i = 0; i < size; i++) alt[i] = main[i];
    for (size_t i = 0; i < size; i++) {
        if (((main[i] & 1U) == 0) && ((main[(i + size - 1) % size] & 1U) != 0) && ((main[i] & L1_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[i] |= 1U;
        else if (((main[i] & 1U) == 0) && ((main[(i + size - 1) % size] & 1U) != 0) && ((main[i] & R1_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[(i + size - 1) % size] &= 30U;
        else if (((main[i] & 1U) != 0) && ((main[(i + size - 1) % size] & 1U) == 0) && ((main[i] & L2_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[i] &= 30U;
        else if (((main[i] & 1U) != 0) && ((main[(i + size - 1) % size] & 1U) == 0) && ((main[i] & R2_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[(i + size - 1) % size] |= 1U;
    }
    this->swap();
}

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "1D cellular automaton");
    auto    n = clp.param("n", size_t(500), "System size");
    auto    e = clp.param("e", 0.03, "Error rate");
    auto    r = clp.param("r", 0.05, "Forget rate");
    clp.finalize();

    vb::Image img(clp.title, {n, n});
    img.show();

    Automaton a(n);

    for (size_t x = 0; x < n; ++x) a.main[x] = vb::prng() & 31U;

    for (size_t i = 0;; ++i) {
        for (size_t x = 0; x < n; ++x) { img.put({x, i % n}, vb::Grey(255 * (a.main[x] & 1U))); }
        for (int j = 0; j < 100; ++j) {
            a.randomize(e);
            a.emit();
            a.shift();
            a.effect(1);
            a.forget(r);
        }
    }
}
