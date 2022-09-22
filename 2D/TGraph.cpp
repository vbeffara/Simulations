#include <vb/Console.h>
#include <vb/Figure.h>
#include <vb/data/Array.h>
#include <vb/util/Hub.h>

using namespace vb;
using namespace std;

struct loc {
    cpx z = 0;
};

class TGraph : public Figure, Array<loc> {
public:
    using Array::size;

    TGraph(const Hub &H, size_t n, cpx A_, double t) : Figure(H.title), Array<loc>({n, n}), A(A_), theta(t) {
        compute();
        plot();
        show();
        output(H.title);
    };

    [[nodiscard]] auto phiwb(int mw, int nw, int mb, int nb) const -> cpx {
        return real(pow(beta / gamma, -mw) * pow(beta / alpha, -nw) / lambda) * alpha * lambda * pow(beta / gamma, mb) *
               pow(beta / alpha, nb);
    }

    void compute() {
        a      = abs(B - A);
        b      = abs(C - B);
        c      = abs(A - C);
        alpha  = (B - A) / a;
        beta   = (C - B) / b;
        gamma  = (A - C) / c;
        pa     = arg((C - A) / (B - A)) / M_PI;
        pb     = arg((A - B) / (C - B)) / M_PI;
        pc     = arg((B - C) / (A - C)) / M_PI;
        lambda = exp(2.0 * M_PI * I * theta);

        for (size_t ii = 0; ii < size.x; ++ii) {
            for (size_t j = 0; j < size.y; ++j) {
                if (j > 0)
                    at({ii, j}).z = at({ii, j - 1}).z + c * phiwb(int(ii) + 1, int(j) - 2, int(ii), int(j) - 1);
                else if (ii > 0)
                    at({ii, j}).z = at({ii - 1, j}).z - a * phiwb(int(ii), int(j) - 1, int(ii), int(j) - 1);
            }
        }
    }

    void plot() {
        contents.clear();
        for (size_t ii = 0; ii < size.x; ++ii) {
            for (size_t j = 0; j < size.y; ++j) {
                if (ii < size.x - 1) add(make_unique<Segment>(at({ii, j}).z, at({ii + 1, j}).z));
                if (j < size.y - 1) add(make_unique<Segment>(at({ii, j}).z, at({ii, j + 1}).z));
                if ((ii > 0) && (j < size.y - 1)) add(make_unique<Segment>(at({ii, j}).z, at({ii - 1, j + 1}).z));
            }
        }
    }

    void loop() {
        while (true) {
            theta = Duration(now() - start).count() / 3;
            compute();
            plot();
            update();
        }
    }

    void interact() {
        Console CC;
        CC.manage(theta, 0.0, 1.0, "arg(λ) / 2π", [this] {
            compute();
            plot();
            update();
        });
        CC.watch(lambda, "λ");
        CC.manage(A, {-1, 0}, {2, 2}, "point A", [this] {
            compute();
            plot();
            update();
        });
        CC.show();
        CC.pause();
    }

    cpx    A, B = 0, C = 1, alpha = 1, beta = 1, gamma = 1, lambda;
    double theta, a = 1, b = 1, c = 1, pa = 1, pb = 1, pc = 1;
};

auto main(int argc, char **argv) -> int {
    Hub const H("T-graph for the triangular lattice", argc, argv, "n=20,l,x=.4,y=.6,t");
    TGraph TG(H, H['n'], cpx{H['x'], H['y']}, H['t']);
    if (H['l'])
        TG.loop();
    else
        TG.interact();
}
