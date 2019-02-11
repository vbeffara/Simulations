#include <vb/Bitmap.h>

#define MAIN_BIT 1u
#define L1_BIT 2u
#define L2_BIT 4u
#define R1_BIT 8u
#define R2_BIT 16u
#define ALL_BITS 31u

class Automaton {
public:
    explicit Automaton(int /*n*/);

    void swap() { main.swap(alt); };

    void randomize(double /*e*/);
    void shift();
    void emit();
    void forget(double r);
    void effect(double r);

    int                  size;
    std::vector<uint8_t> main;
    std::vector<uint8_t> alt;
};

Automaton::Automaton(int n) : size(n), main(n, 0), alt(n) {}

void Automaton::randomize(double e) {
    for (int i = 0; i < size; ++i)
        if (vb::prng.bernoulli(e)) main[i] = vb::prng() & ALL_BITS;
}

void Automaton::shift() {
    for (int i = 0; i < size; ++i)
        alt[i] = (main[i] & MAIN_BIT) + (main[(i + size - 1) % size] & R1_BIT) + (main[(i + size - 1) % size] & R2_BIT) +
                 (main[(i + 1) % size] & L1_BIT) + (main[(i + 1) % size] & L2_BIT);
    this->swap();
}

void Automaton::emit() {
    for (int i = 0; i < size; i++) {
        alt[i] = main[i];
        if (((main[i] & 1u) != 0) && ((main[(i + size - 1) % size] & 1u) == 0)) alt[i] |= L1_BIT | R1_BIT;
        if (((main[i] & 1u) == 0) && ((main[(i + size - 1) % size] & 1u) != 0)) alt[i] |= L2_BIT | R2_BIT;
    }
    this->swap();
}

void Automaton::forget(double r) {
    for (int i = 0; i < size; ++i)
        if (vb::prng.bernoulli(r)) main[i] &= 1u;
}

void Automaton::effect(double r) {
    for (int i = 0; i < size; i++) alt[i] = main[i];
    for (int i = 0; i < size; i++) {
        if (((main[i] & 1u) == 0) && ((main[(i + size - 1) % size] & 1u) != 0) && ((main[i] & L1_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[i] |= 1u;
        else if (((main[i] & 1u) == 0) && ((main[(i + size - 1) % size] & 1u) != 0) && ((main[i] & R1_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[(i + size - 1) % size] &= 30u;
        else if (((main[i] & 1u) != 0) && ((main[(i + size - 1) % size] & 1u) == 0) && ((main[i] & L2_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[i] &= 30u;
        else if (((main[i] & 1u) != 0) && ((main[(i + size - 1) % size] & 1u) == 0) && ((main[i] & R2_BIT) != 0) && (vb::prng.bernoulli(r)))
            alt[(i + size - 1) % size] |= 1u;
    }
    this->swap();
}

int main(int argc, char **argv) {
    vb::Hub      H("1D cellular automaton", argc, argv, "n=500,e=.03,r=.05");
    const int    n = H['n'];
    const double e = H['e'];
    const double r = H['r'];

    vb::Image img(H, {n, n});
    img.show();

    Automaton a(n);

    for (int x = 0; x < n; ++x) a.main[x] = vb::prng() & 31u;

    for (int i = 0;; ++i) {
        int nb = 0;
        for (int x = 0; x < n; ++x) {
            img.put({x, i % n}, vb::Grey(255 * (a.main[x] & 1u)));
            nb += a.main[x] & 1u;
        }
        //    img.update();
        for (int j = 0; j < 100; ++j) {
            a.randomize(e);
            a.emit();
            a.shift();
            a.effect(1);
            a.forget(r);
        }
    }
}
