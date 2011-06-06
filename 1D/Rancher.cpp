/**
 * Simulation de la "marche aléatoire qui évite son enveloppe convexe
 * passée" avec condition initiale donnée par un cône ; avec dessin des
 * enveloppes successives
 *
 * Output vectoriel en PDF (ou PS)
 *
 * gcc -DMEXP=19937 /usr/include/SFMT/SFMT.c rancher.c -o rancher -lm
 *
 * USAGE : rancher pente nb inter filename [x]
 *
 * pente (float) tangente du demi-angle initial
 * nb steps (int)
 * inter (int) : inverse de la fréquence d'affichage des enveloppes
 * filename : nom (sans extension !) du fichier de sortie
 * x : si paramètre présent, output en .ps, sinon en .pdf
 */

#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace std;
using namespace vb;

class point {
public:
  point (double _x, double _y, char _z) : x(_x), y(_y), z(_z) {};
  double x;
  double y;
  char z;   ///< 0 si point à l'infini, 1 si point du plan (cf. plan projectif)
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

  if (A.z) vxa=A.x-O.x, vya=A.y-O.y; else vxa=A.x, vya=A.y;
  if (B.z) vxb=B.x-O.x, vyb=B.y-O.y; else vxb=B.x, vyb=B.y;

  double output = atan2(vyb,vxb) - atan2(vya,vxa);
  if (output>M_PI) output -= 2*M_PI;
  return output;
}

point rand_point (maillon & position) {
  point p = position.p, pp = position.prev->p;
  double vxp,vyp;

  if (pp.z) vxp = pp.x-p.x, vyp = pp.y-p.y;
  else      vxp = pp.x,     vyp = pp.y;

  double u = hypot(vxp,vyp); vxp /= u; vyp /= u;

  double alpha    = prng.uniform_real(0, 2*M_PI + angle(position.p, position.prev->p, position.suiv->p));
  double sinalpha = sin(alpha);
  double cosalpha = cos(alpha);

  return point (p.x+vxp*cosalpha-vyp*sinalpha, p.y+vxp*sinalpha+vyp*cosalpha, 1);
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

  void dessine_enveloppe (FILE *fichier, maillon *debut) {
    maillon *next=debut;
    double x,y,ox,oy;
    double r,g,b;

    fprintf(fichier,"%f %f %f setrgbcolor\n",r=prng.uniform_real(),g=prng.uniform_real(),b=prng.uniform_real());
    P = Color (256*r, 256*g, 256*b);
    fprintf(fichier,"newpath\n");

    if (debut->p.z) x = debut->p.x,                       y = debut->p.y;
    else            x = debut->suiv->p.x + debut->p.x/10, y = debut->suiv->p.y + debut->p.y/10;

    fprintf(fichier, "%f %f moveto\n", x, y);

    while ((next=next->suiv)) {
      ox=x; oy=y;
      if (next->p.z) x = next->p.x,                      y = next->p.y;
      else           x = next->prev->p.x + next->p.x/10, y = next->prev->p.y + next->p.y/10;

      fprintf(fichier, "%f %f lineto\n", x, y);
      F.add (new Segment (cpx(ox,oy), cpx(x,y), P));
    }
    fprintf(fichier,"stroke\n");
    F.step();
  }

  void main (int argc, char ** argv) {
    if (argc<5) {
      printf ("Usage : rancher pente nb inter filename [x]\n");
      printf ("Produit le fichier filename.pdf (ou .ps si paramètre 'x' présent)\n");
      exit(1);
    }

    double pente;       sscanf(argv[1],"%lf",&pente);
    int nb;             sscanf(argv[2],"%d",&nb);
    int inter;          sscanf(argv[3],"%d",&inter);
    char filename[255]; sprintf(filename,"%s.ps",argv[4]);

    F.add (new Segment (cpx(0,0), cpx(1,1)));
    F.show();

    FILE *fichier = tmpfile();

    double minx=+INFINITY, maxx=-INFINITY, miny=+INFINITY, maxy=-INFINITY;

    traj.resize(nb,point(0,0,0));
    traj[0] = point (-2*nb, -pente*2*nb, 0);
    traj[1] = point (-2*nb, pente*2*nb, 0);
    traj[2] = point (0, 0, 1);

    maillon debut (traj[0]), fin (traj[1]);

    maillon * position = new maillon (traj[2], &debut, &fin);

    debut.suiv=position;
    fin.prev=position;

    for (int i=3; i<nb; i++) {
      traj[i] = rand_point (*position);

      if (traj[i].x>maxx) maxx=traj[i].x;
      if (traj[i].x<minx) minx=traj[i].x;
      if (traj[i].y>maxy) maxy=traj[i].y;
      if (traj[i].y<miny) miny=traj[i].y;

      position = new maillon(traj[i]);

      insere_maillon(*position, &debut);
      if (!(i%inter)) dessine_enveloppe(fichier, &debut);
    }
    fprintf(fichier, "stroke");

    rewind(fichier);

    FILE * fichier_final = fopen(filename,"w");

    minx-=(maxx-minx)*0.05; // Pour montrer la situation initiale (cône)

    fprintf(fichier_final,"%%!\n%%%%DocumentMedia: A4 595 842 80 white ( )\n%%%%Orientation: Portrait\n");
    float scale,scalex,scaley,transx,transy;
    scalex=842./(maxx-minx);
    scaley=595./(maxy-miny);
    scale=(scalex<scaley)?scalex:scaley;
    transx=(595-(maxy-miny)*scale)/2+maxy*scale;
    transy=(842-(maxx-minx)*scale)/2-minx*scale;
    fprintf(fichier_final,"%f %f translate\n", transx, transy);
    fprintf(fichier_final,"%f %f scale\n", scale, scale);
    fprintf(fichier_final,"90 rotate\n");

    // Copie de fichier vers fichier_final
    char line[256];
    while(fgets(line, 256, fichier)!=NULL)
      fputs(line, fichier_final);

    fclose(fichier);
    fclose(fichier_final);

    if(argc==5) {
      char commande[256];
      sprintf(commande,"ps2pdf %s",filename);
      system(commande);

      sprintf(commande,"rm %s",filename);
      system(commande);
    }

    F.pause();
    F.output_pdf("toto2");
  }
};

int main (int argc, char **argv) {
  Rancher R;
  R.main(argc,argv);
  return 0;
}
