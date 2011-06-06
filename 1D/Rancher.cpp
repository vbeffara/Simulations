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
  Figure F;
  Pen P;

  // Ajoute le point position à l'enveloppe convexe débutant par debut
  // (traj est le tableau des coordonnées des points) (le paramètre fin
  // est en fait inutile)
  void insere_maillon (maillon & position, maillon *debut) {
    maillon *maillonmax, *maillonmin;
    maillon *next = debut;

    double minpente = +INFINITY; // M_PI devrait suffire
    double maxpente = -INFINITY; // -M_PI devrait suffire

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

    // Desallocation des chaînons intermédiaires
    libere_maillons(maillonmax,maillonmin);

    // Introduction de position dans la chaîne, entre maillonmin et maillonmax
    position.prev = maillonmax;
    position.suiv = maillonmin;
    maillonmax -> suiv = &position;
    maillonmin -> prev = &position;
  }

  void dessine_enveloppe (maillon *m) {
    vector<cpx> V;

    P = Color (128+prng.uniform_int(128), 128+prng.uniform_int(128), 128+prng.uniform_int(128));

    if (m->p.k) V.push_back (m->p);
    else V.push_back (cpx(0, cpx(m->suiv->p).imag() + cpx(m->suiv->p).real() * cpx(m->p).imag()));

    while ((m = m->suiv))
      if (m->p.k) V.push_back (m->p);
      else V.push_back (cpx(0, cpx(m->prev->p).imag() + cpx(m->prev->p).real() * cpx(m->p).imag()));

    F.add (new Path(V,P));
  }

  void main (int argc, char ** argv) {
    CL_Parser CLP (argc, argv, "p=.1,n=1000,i=1");

    double pente = CLP('p');
    int    nb    = CLP('n');
    int    inter = CLP('i');

    traj.push_back (point (cpx(-1, -pente), 0));
    traj.push_back (point (cpx(-1, pente), 0));
    traj.push_back (point (cpx(0, 0)));

    maillon debut (traj[0]), fin (traj[1]);

    maillon * position = new maillon (traj[2], &debut, &fin);

    debut.suiv=position; fin.prev=position;

    for (int i=3; i<nb; i++) {
      traj.push_back (rand_point (*position));
      position = new maillon(traj[i]);
      insere_maillon(*position, &debut);
      if (!((i+1)%inter)) dessine_enveloppe(&debut);
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
