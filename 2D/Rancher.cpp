/**
 * Simulation de la "marche aléatoire qui évite son enveloppe convexe
 * passée" avec condition initiale donnée par un cône ; avec dessin des
 * enveloppes successives
 *
 * pente (float) tangente du demi-angle initial
 * nb steps (int)
 * inter (int) : inverse de la fréquence d'affichage des enveloppes
 */

#include <boost/next_prior.hpp>
#include <list>
#include <vb/Console.h>
#include <vb/Figure.h>
#include <vb/util/PRNG.h>

using vb::cpx;

class point {
public:
    explicit point(cpx _z, bool _k = true) : z(_z), k(_k){};
    cpx  z;
    bool k;
};

double angle(const point &O, const point &A, const point &B) {
    cpx vza = A.z;
    if (A.k) vza -= O.z;
    cpx vzb = B.z;
    if (B.k) vzb -= O.z;
    double output = arg(vzb) - arg(vza);
    if (output > M_PI) output -= 2 * M_PI;
    return output;
}

class Rancher {
public:
    std::vector<point>         traj;
    std::list<point>           env;
    std::list<point>::iterator cur;
    vb::Hub                    H;
    vb::Figure                 F;
    vb::Pen                    P;
    vb::Console                W;

    Rancher(int argc, char **argv) : H("Rancher process", argc, argv, "p=.1,n=1000,i=1,o,r"), F(H), W(H) {}

    point rand_point() {
        point p = *cur, pp = *boost::prior(cur), ppp = *boost::next(cur);
        cpx   vzp = pp.z;
        if (pp.k) vzp -= p.z;
        double alpha = vb::prng.uniform_real(0, 2 * M_PI + angle(p, pp, ppp));

        return point(p.z + vzp * std::polar(1.0, alpha) / sqrt(norm(vzp)));
    }

    std::list<point>::iterator insere_maillon(const point &p) {
        std::list<point>::iterator maillonmin2, maillonmax2, i;
        double                     minpente2 = +INFINITY, maxpente2 = -INFINITY;

        for (i = env.begin(); i != env.end(); ++i) {
            double pente = angle(p, *(env.begin()), *i);
            if (pente <= minpente2) {
                minpente2   = pente;
                maillonmin2 = i;
            }
        }

        i = maillonmin2;

        do {
            --i;
            double pente = angle(p, *(env.begin()), *i);
            if (pente >= maxpente2) {
                maxpente2   = pente;
                maillonmax2 = i;
            }
        } while (i != env.begin());

        env.erase(boost::next(maillonmax2), maillonmin2);
        return env.insert(maillonmin2, p);
    }

    void dessine_enveloppe() {
        std::vector<cpx> V;

        P = vb::Pen(vb::Color(128 + vb::prng.uniform_int(128), 128 + vb::prng.uniform_int(128), 128 + vb::prng.uniform_int(128)));

        for (auto i = env.begin(); i != env.end(); ++i) {
            if (i == env.begin()) {
                auto j = i;
                ++j;
                V.emplace_back(0, (j->z).imag() + (j->z).real() * (i->z).imag());
            } else if (!(i->k)) {
                auto j = i;
                --j;
                V.emplace_back(0, (j->z).imag() + (j->z).real() * (i->z).imag());
            } else
                V.push_back(i->z);
        }

        F.add(std::make_unique<vb::Path>(V, P));
    }

    double env_width() {
        auto i = env.begin();
        ++i;
        point p1 = *i;
        i        = env.end();
        --i;
        --i;
        point p2 = *i;
        return sqrt(norm(p1.z - p2.z));
    }

    void main() {
        double pente = H['p'];
        int    nb    = H['n'];
        int    inter = H['i'];
        bool   plot  = H['o'];
        bool   renew = H['r'];

        int i = 0;

        W.watch(i, "Path length");
        W.lambda<int>([this] { return env.size(); }, "Envelope size");
        W.lambda<double>([this] { return env_width(); }, "Envelope width");
        W.show();

        traj.emplace_back(cpx(-1, -pente), false);
        traj.emplace_back(cpx(-1, pente), false);
        traj.emplace_back(cpx(0, 0));

        env.push_back(traj[0]);
        env.push_back(traj[2]);
        env.push_back(traj[1]);

        cur = env.begin();
        ++cur;

        int              last_renewal = 3;
        std::vector<int> renewals;

        for (i = 3; i < nb; i++) {
            point p = rand_point();
            if (plot) traj.push_back(p);
            cur = insere_maillon(p);
            if (plot && ((i + 1) % inter == 0)) dessine_enveloppe();
            W.step();

            if (renew && (env.size() == 3)) {
                unsigned r = i - last_renewal;
                if (r + 1 > renewals.size()) renewals.resize(r + 1);
                renewals[r]++;
                last_renewal = i;
            }
        }

        if (plot) {
            std::vector<cpx> path;
            for (unsigned i = 2; i < traj.size(); ++i) path.push_back(traj[i].z);
            F.add(std::make_unique<vb::Path>(path));
            F.show();
            F.pause();
            F.output_pdf(H, "Rancher");
        }

        if (renew) {
            int n = 0, sx = 0, sxx = 0;
            for (unsigned i = 0; i < renewals.size(); ++i) {
                n += renewals[i];
                sx += i * renewals[i];
                sxx += i * i * renewals[i];
            }
            std::cerr << n << " renewals (density " << double(n) / nb << ")" << std::endl;
            std::cerr << "Average length: " << double(sx) / n << std::endl;
        }
    }
};

int main(int argc, char **argv) { Rancher(argc, argv).main(); }
