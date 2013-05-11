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
#include <vb/Console.h>

using namespace std;
using namespace vb;

class point {
  cpx z;
public:
  point (cpx _z, bool _k=1) : z(_z), k(_k) {};
  operator cpx() const { return z; }
  bool k; ///< 0 si point à l'infini, 1 si point du plan (cf. plan projectif)
};

double angle (const point &O, const point &A, const point &B) {
  cpx vza = A; if (A.k) vza -= cpx(O);
  cpx vzb = B; if (B.k) vzb -= cpx(O);
  double output = arg(vzb) - arg(vza);
  if (output>M_PI) output -= 2*M_PI;
  return output;
}

int env_size (void*);
double env_width (void*);

class Rancher {
public:
  vector<point> traj;
  list<point> env;
  list<point>::iterator cur;
  Figure F;
  Pen P;

  point rand_point () {
    point p = *cur, pp = *boost::prior(cur), ppp = *boost::next(cur);
    cpx vzp = pp; if (pp.k) vzp -= cpx(p);
    double alpha = prng.uniform_real(0, 2*M_PI + angle(p, pp, ppp));

    return cpx(p) + vzp*polar(1.0,alpha)/sqrt(norm(vzp));
  }

  list<point>::iterator insere_maillon (point & p) {
    list<point>::iterator maillonmin2, maillonmax2, i;
    double minpente2 = +INFINITY, maxpente2 = -INFINITY;

    for (i = env.begin(); i != env.end(); ++i) {
      double pente = angle (p, *(env.begin()), *i);
      if (pente <= minpente2) {
        minpente2 = pente;
        maillonmin2 = i;
      }
    }

    i = maillonmin2;

    do { --i;
      double pente = angle (p, *(env.begin()), *i);
      if (pente >= maxpente2) {
        maxpente2 = pente;
        maillonmax2 = i;
      }
    } while (i != env.begin());

    env.erase (boost::next(maillonmax2), maillonmin2);
    return env.insert (maillonmin2, p);
  }

  void dessine_enveloppe () {
    vector<cpx> V;

    P = Color (128+prng.uniform_int(128), 128+prng.uniform_int(128), 128+prng.uniform_int(128));

    for (list<point>::iterator i = env.begin(); i != env.end(); ++i) {
      if (i == env.begin()) {
        list<point>::iterator j = i; ++j;
        V.push_back (cpx(0, cpx(*j).imag() + cpx(*j).real() * cpx(*i).imag()));
      } else if (!(i->k)) {
        list<point>::iterator j = i; --j;
        V.push_back (cpx(0, cpx(*j).imag() + cpx(*j).real() * cpx(*i).imag()));
      } else V.push_back (*i);
    }

    F.add (new Path(V,P));
  }

  void main (int argc, char ** argv) {
    CL_Parser CLP (argc, argv, "p=.1,n=1000,i=1,o,r");
    double pente = CLP('p');
    int    nb    = CLP('n');
    int    inter = CLP('i');
    bool   plot  = CLP('o');
    bool   renew = CLP('r');

    int i=0;

    Console *W = new Console;
    W->watch (i,"Path length");
    W->trace (env_size,this,"Envelope size");
    W->trace (env_width,this,"Envelope width");
    W->show();

    traj.push_back (point (cpx(-1, -pente), 0));
    traj.push_back (point (cpx(-1, pente), 0));
    traj.push_back (point (cpx(0, 0)));

    env.push_back (traj[0]);
    env.push_back (traj[2]);
    env.push_back (traj[1]);

    cur = env.begin(); ++cur;

    int last_renewal=3;
    vector<int> renewals;

    for (i=3; i<nb; i++) {
      point p = rand_point();
      if (plot) traj.push_back (p);
      cur = insere_maillon(p);
      if (plot && !((i+1)%inter)) dessine_enveloppe();
      W->step();

      if (renew && (env_size(this)==3)) {
        int r = i-last_renewal;
        if (r+1>renewals.size())
          renewals.resize(r+1);
        renewals[r] ++;
        last_renewal = i;
      }
    }

    delete W;

    if (plot) {
      vector<cpx> path; for (int i=2; i<traj.size(); ++i) path.push_back (traj[i]);
      F.add (new Path(path));
      F.show(); F.pause();
      F.output_pdf("Rancher");
    }

    if (renew) {
      long int n=0, sx=0, sxx=0;
      for (int i=0; i<renewals.size(); ++i) {
        n += renewals[i];
        sx += i*renewals[i];
        sxx += i*i*renewals[i];
      }
      cerr << n << " renewals (density " << double(n)/nb << ")" << endl;
      cerr << "Average length: " << double(sx)/n << endl;
    }
  }
};

int env_size (void *R) {
  Rancher & RR = * (Rancher*) R;
  return RR.env.size();
}

double env_width (void *R) {
  Rancher & RR = * (Rancher*) R;
  list<point> L = RR.env;
  list<point>::iterator i = L.begin(); ++i;
  point p1 = *i;
  i = L.end(); --i; --i;
  point p2 = *i;
  return sqrt (norm (cpx(p1) - cpx(p2)));
}

int main (int argc, char **argv) {
  Rancher().main(argc,argv);
  return 0;
}
