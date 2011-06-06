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
public:
  point (cpx _z, bool _k) : z(_z), k(_k) {};
  cpx z;
  bool k;   ///< 0 si point à l'infini, 1 si point du plan (cf. plan projectif)
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
double angle (point &O, point &A, point &B) {
  double vxa,vya,vxb,vyb;

  if (A.k) vxa=A.z.real() - O.z.real(), vya=A.z.imag()-O.z.imag(); else vxa=A.z.real(), vya=A.z.imag();
  if (B.k) vxb=B.z.real()-O.z.real(), vyb=B.z.imag()-O.z.imag(); else vxb=B.z.real(), vyb=B.z.imag();

  double output = atan2(vyb,vxb) - atan2(vya,vxa);
  if (output>M_PI) output -= 2*M_PI;
  return output;
}

point rand_point (maillon & position) {
  point p = position.p, pp = position.prev->p;
  double vxp,vyp;

  if (pp.k) vxp = pp.z.real()-p.z.real(), vyp = pp.z.imag()-p.z.imag();
  else      vxp = pp.z.real(),     vyp = pp.z.imag();

  double u = hypot(vxp,vyp); vxp /= u; vyp /= u;

  double alpha    = prng.uniform_real(0, 2*M_PI + angle(position.p, position.prev->p, position.suiv->p));
  double sinalpha = sin(alpha);
  double cosalpha = cos(alpha);

  return point (cpx(p.z.real()+vxp*cosalpha-vyp*sinalpha, p.z.imag()+vxp*sinalpha+vyp*cosalpha), 1);
}

// Libère (en mémoire) les maillons strictement compris entre début et
// fin
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
    double pente;

    maillon *next = debut;

    double minpente = +INFINITY; // M_PI devrait suffire
    double maxpente = -INFINITY; // -M_PI devrait suffire

    while (next) {
      pente = angle (position.p, debut->p, next->p);
      if (pente <= minpente) {
        minpente = pente;
        maillonmin = next;
      }
      next = next -> suiv;
    }

    next = maillonmin;

    while (next) {
      pente = angle (position.p, debut->p, next->p);
      if (pente >= maxpente) {
        maxpente = pente;
        maillonmax = next;
      }
      next= next -> prev;
    }

    // Desallocation des chaînons intermédiaires
    libere_maillons(maillonmax,maillonmin);

    // Introduction de position dans la chaîne, entre maillonmin et maillonmax
    position.prev = maillonmax;
    position.suiv = maillonmin;
    maillonmax -> suiv = &position;
    maillonmin -> prev = &position;
  }

  void dessine_enveloppe (maillon *debut) {
    maillon *next=debut;
    double x,y,ox,oy;

    P = Color (128+prng.uniform_int(128), 128+prng.uniform_int(128), 128+prng.uniform_int(128));

    if (debut->p.k) x = debut->p.z.real(), y = debut->p.z.imag();
    else            x = 0, y = debut->suiv->p.z.imag() + debut->suiv->p.z.real() * debut->p.z.imag();

    while ((next=next->suiv)) {
      ox=x; oy=y;
      if (next->p.k) x = next->p.z.real(), y = next->p.z.imag();
      else           x = 0, y = next->prev->p.z.imag() + next->prev->p.z.real() * next->p.z.imag();

      F.add (new Segment (cpx(ox,oy), cpx(x,y), P));
    }
  }

  void main (int argc, char ** argv) {
    CL_Parser CLP (argc, argv, "p=.1,n=1000,i=1");

    double pente = CLP('p');
    int nb = CLP('n');
    int inter = CLP('i');

    traj.push_back (point (cpx(-1, -pente), 0));
    traj.push_back (point (cpx(-1, pente), 0));
    traj.push_back (point (cpx(0, 0), 1));

    maillon debut (traj[0]), fin (traj[1]);

    maillon * position = new maillon (traj[2], &debut, &fin);

    debut.suiv=position;
    fin.prev=position;

    for (int i=3; i<nb; i++) {
      traj.push_back (rand_point (*position));
      position = new maillon(traj[i]);
      insere_maillon(*position, &debut);
      if (!((i+1)%inter)) {
        dessine_enveloppe(&debut);
      }
    }

    vector<cpx> path;
    for (int i=0; i<traj.size(); ++i) path.push_back (cpx(traj[i].z.real(), traj[i].z.imag()));
    F.add (new Path(path));
    F.show();
    F.pause();
    F.output_pdf("Rancher");
  }
};

int main (int argc, char **argv) {
  Rancher R;
  R.main(argc,argv);
  return 0;
}
