/**
 * Simulation de la "marche aléatoire qui évite son enveloppe convexe
 * passée" avec condition initiale donnée par un cône ; avec dessin des
 * enveloppes successives
 *
 * pente (float) tangente du demi-angle initial
 * nb steps (int)
 * inter (int) : inverse de la fréquence d'affichage des enveloppes
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace std;
using namespace vb;

class point {
  cpx z;
public:
  point (cpx _z, bool _k=1) : z(_z), k(_k) {};
  operator cpx() const { return z; }
  bool k; ///< 0 si point à l'infini, 1 si point du plan (cf. plan projectif)
};

// Angle (O,A), (O,B) entre -pi et pi. Suppose que O n'est pas à
// l'infini
double angle (const point &O, const point &A, const point &B) {
  cpx vza = A; if (A.k) vza -= cpx(O);
  cpx vzb = B; if (B.k) vzb -= cpx(O);

  double output = arg(vzb) - arg(vza);
  if (output>M_PI) output -= 2*M_PI;
  return output;
}

class Rancher {
public:
  vector<point> traj; ///< Coordonnées des points
  list<point>   env;
  list<point>::iterator cur;
  Figure F;
  Pen P;

  point rand_point () {
    point p = *cur;
    list<point>::iterator i = cur; --i;
    point pp = *i;
    ++i; ++i;
    point ppp = *i;

    cpx    vzp   = pp; if (pp.k) vzp -= cpx(p);
    double alpha = prng.uniform_real(0, 2*M_PI + angle(p, pp, ppp));

    return cpx(p) + vzp*polar(1.0,alpha)/sqrt(norm(vzp));
  }

  // Ajoute le point position à l'enveloppe convexe débutant par debut
  // (traj est le tableau des coordonnées des points) (le paramètre fin
  // est en fait inutile)
  list<point>::iterator insere_maillon (point & p) {
    list<point>::iterator maillonmin2, maillonmax2;
    list<point>::iterator i;

    double minpente2 = +INFINITY;
    double maxpente2 = -INFINITY;

    i = env.begin();

    while (i != env.end()) {
      double pente = angle (p, *(env.begin()), *i);
      if (pente <= minpente2) {
        minpente2 = pente;
        maillonmin2 = i;
      }
      i++;
    }

    i = maillonmin2;

    while (i != env.begin()) {
      double pente = angle (p, *(env.begin()), *i);
      if (pente >= maxpente2) {
        maxpente2 = pente;
        maillonmax2 = i;
      }
      --i;
    }

    if (i == env.begin()) {
      double pente = angle (p, *(env.begin()), *i);
      if (pente >= maxpente2) {
        maxpente2 = pente;
        maillonmax2 = i;
      }
    }

    i = maillonmax2; ++i;
    list<point>::iterator j = maillonmin2; // --j;
    env.erase (i,j);

    i = maillonmax2; ++i;
    return env.insert (i, p);
  }

  void dessine_enveloppe () {
    vector<cpx> VV;

    P = Color (128+prng.uniform_int(128), 128+prng.uniform_int(128), 128+prng.uniform_int(128));

    for (list<point>::iterator i = env.begin(); i != env.end(); ++i) {
      if (i == env.begin()) {
        list<point>::iterator j = i; ++j;
        VV.push_back (cpx(0, cpx(*j).imag() + cpx(*j).real() * cpx(*i).imag()));
      } else if (!(i->k)) {
        list<point>::iterator j = i; --j;
        VV.push_back (cpx(0, cpx(*j).imag() + cpx(*j).real() * cpx(*i).imag()));
      } else VV.push_back (*i);
    }

    F.add (new Path(VV,P));
  }

  void main (int argc, char ** argv) {
    CL_Parser CLP (argc, argv, "p=.1,n=1000,i=1");

    double pente = CLP('p');
    int    nb    = CLP('n');
    int    inter = CLP('i');

    traj.push_back (point (cpx(-1, -pente), 0));
    traj.push_back (point (cpx(-1, pente), 0));
    traj.push_back (point (cpx(0, 0)));

    env.push_back (traj[0]);
    env.push_back (traj[2]);
    env.push_back (traj[1]);
    cur = env.begin(); ++cur;

    for (int i=3; i<nb; i++) {
      point p = rand_point();
      traj.push_back (p);
      cur = insere_maillon(p);
      if (!((i+1)%inter)) dessine_enveloppe();
    }

    vector<cpx> path; for (int i=0; i<traj.size(); ++i) path.push_back (traj[i]);
    F.add (new Path(path));
    F.show(); F.pause();
    F.output_pdf("Rancher");
  }
};

int main (int argc, char **argv) {
  Rancher R;
  R.main(argc,argv);
  return 0;
}
