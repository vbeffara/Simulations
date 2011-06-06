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

// L'enveloppe convexe est décrite par une liste doublement chaînée
// (père & fils), qui la parcourt dans le sens positif (i.e. centre de
// courbure à gauche)

class maillon {
public:
  maillon (point _p, maillon *_prev=0, maillon *_suiv=0) : p(_p), prev(_prev), suiv(_suiv) {};
  point p;
  maillon *prev;
  maillon *suiv;
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

point rand_point (const maillon & position) {
  point p = position.p, pp = position.prev->p;

  cpx    vzp   = pp; if (pp.k) vzp -= cpx(p);
  double alpha = prng.uniform_real(0, 2*M_PI + angle(position.p, position.prev->p, position.suiv->p));

  return cpx(p) + vzp*polar(1.0,alpha)/sqrt(norm(vzp));
}

// Libère (en mémoire) les maillons strictement compris entre début et fin
void libere_maillons (maillon *debut, maillon *fin) {
  maillon *m = debut->suiv;
  while (m!=fin) {
    maillon *next = m->suiv;
    delete m;
    m = next;
  }
}

class Rancher {
public:
  vector<point> traj; ///< Coordonnées des points
  list<point>   env;
  list<point>::iterator cur;
  Figure F;
  Pen P;
  maillon *debut;

  // Ajoute le point position à l'enveloppe convexe débutant par debut
  // (traj est le tableau des coordonnées des points) (le paramètre fin
  // est en fait inutile)
  void insere_maillon (maillon & position) {
    maillon *maillonmax, *maillonmin;
    list<point>::iterator maillonmin2, maillonmax2;
    list<point>::iterator i;

    maillon *next = debut;

    double minpente = +INFINITY; // M_PI devrait suffire
    double maxpente = -INFINITY; // -M_PI devrait suffire
    double minpente2 = +INFINITY;
    double maxpente2 = -INFINITY;

    while (next) {
      double pente = angle (position.p, debut->p, next->p);
      if (pente <= minpente) {
        minpente = pente;
        maillonmin = next;
      }
      next = next -> suiv;
    }

    next = maillonmin;

    while (next) {
      double pente = angle (position.p, debut->p, next->p);
      if (pente >= maxpente) {
        maxpente = pente;
        maillonmax = next;
      }
      next = next -> prev;
    }

    i = env.begin();

    while (i != env.end()) {
      double pente = angle (position.p, debut->p, *i);
      if (pente <= minpente2) {
        minpente2 = pente;
        maillonmin2 = i;
      }
      i++;
    }

    i = maillonmin2;

    while (i != env.begin()) {
      double pente = angle (position.p, debut->p, *i);
      if (pente >= maxpente2) {
        maxpente2 = pente;
        maillonmax2 = i;
      }
      --i;
    }

    if (i == env.begin()) {
      double pente = angle (position.p, debut->p, *i);
      if (pente >= maxpente2) {
        maxpente2 = pente;
        maillonmax2 = i;
      }
    }

    // Desallocation des chaînons intermédiaires
    libere_maillons(maillonmax,maillonmin);

    i = maillonmax2; ++i;
    list<point>::iterator j = maillonmin2; // --j;
    env.erase (i,j);

    // Introduction de position dans la chaîne, entre maillonmin et maillonmax
    position.prev = maillonmax;
    position.suiv = maillonmin;
    maillonmax -> suiv = &position;
    maillonmin -> prev = &position;

    i = maillonmax2; ++i;
    env.insert (i, position.p);
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

    maillon deb (traj[0]), fin (traj[1]);
    debut = &deb;

    maillon * position = new maillon (traj[2], debut, &fin);

    deb.suiv=position; fin.prev=position;

    env.push_back (deb.p);
    env.push_back (position->p);
    env.push_back (fin.p);
    cur = env.begin(); ++cur;

    for (int i=3; i<nb; i++) {
      assert (cpx(*cur) == cpx(position->p));
      traj.push_back (rand_point (*position));
      position = new maillon(traj[i]);
      insere_maillon(*position);
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
