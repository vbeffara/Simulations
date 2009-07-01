/*
 * Plonger  dans R^3  une  triangulation periodique  du  plan par  des
 * triangles équilateraux ... Celle de l'exemple c'est celle de période
 *
 * +--+--+
 * | /|\ |
 * |/ | \|
 * +--+--+
 * |\ | /|
 * | \|/ |
 * +--+--+
 *
 * 678
 * 345
 * 012
 */

#define EC cerr<<".\n"

#include <iostream>
#include <time.h>
#include <math.h>

using namespace std;

using std::cout;
using std::cerr;

#define x coo[0]
#define y coo[1]
#define z coo[2]
#define r coo[3]

typedef double costfunction (void);

#define n_total 9

#define nb_bits 256

double coo[4][n_total];
double per[4][2];

int adj_matrix[n_total][n_total];

void edge(int i, int j) {
  adj_matrix[i][j] = 1;
}

void init_lattice (void) {
  edge(0,1);
  edge(1,2);
  edge(3,4);
  edge(4,5);
  edge(6,7);
  edge(7,8);

  edge(0,3);
  edge(3,6);
  edge(1,4);
  edge(4,7);
  edge(2,5);
  edge(5,8);

  edge(0,4);
  edge(2,4);
  edge(4,6);
  edge(4,8);
}

void init_coo (void) {
  for (int i=0;i<n_total;i++) {
    x[i] = i%3;
    y[i] = i/3;
    z[i] = 0;
    r[i] = 0.5;
  }
}

void update_lattice (int i = -1) {
  int j;

  // Compute periods

  switch (i) {
  case -1:
  case 0:
    for (j=0;j<3;j++) {
      per[j][0] = coo[j][2] - coo[j][0];
      per[j][1] = coo[j][6] - coo[j][0];
    }
    break;
  case 2:
    for (j=0;j<3;j++) {
      per[j][0] = coo[j][2] - coo[j][0];
    }
    break;
  case 6:
    for (j=0;j<3;j++) {
      per[j][1] = coo[j][6] - coo[j][0];
    }
  }

  // Update coordinates 

  switch (i) {
  case -1:
    for (j=0;j<3;j++) {
      coo[j][5] = coo[j][3] + per[j][0];
      coo[j][7] = coo[j][1] + per[j][1];
      coo[j][8] = coo[j][2] + per[j][1];
    }
    r[2] = r[0];
    r[5] = r[3];
    r[6] = r[0];
    r[7] = r[1];
    r[8] = r[0];
    break;
  case 0:
    for (j=0;j<3;j++) {
      coo[j][5] = coo[j][3] + per[j][0];
      coo[j][7] = coo[j][1] + per[j][1];
      coo[j][8] = coo[j][2] + per[j][1];
    }
    r[2] = r[0];
    r[6] = r[0];
    r[8] = r[0];
    break;
  case 1:
    for (j=0;j<3;j++) {
      coo[j][7] = coo[j][1] + per[j][1];
    }
    r[7] = r[1];
    break;
  case 2:
    for (j=0;j<3;j++) {
      coo[j][5] = coo[j][3] + per[j][0];
      coo[j][8] = coo[j][2] + per[j][1];
    }
    r[2] = r[0];
    break;
  case 3:
    for (j=0;j<3;j++) {
      coo[j][5] = coo[j][3] + per[j][0];
    }
    r[5] = r[3];
    break;
  case 4:
    break;
  case 5:
    for (j=0;j<3;j++) {
      coo[j][5] = coo[j][3] + per[j][0];
    }
    r[5] = r[3];
    break;
  case 6:
    for (j=0;j<3;j++) {
      coo[j][7] = coo[j][1] + per[j][1];
      coo[j][8] = coo[j][2] + per[j][1];
    }
    r[6] = r[0];
    break;
  case 7:
    for (j=0;j<3;j++) {
      coo[j][7] = coo[j][1] + per[j][1];
    }
    r[7] = r[1];
    break;
  case 8:
    for (j=0;j<3;j++) {
      coo[j][8] = coo[j][2] + per[j][1];
    }
    r[8] = r[0];
    break;
  }
}

void dump_coo (void) {
  for (int i=0;i<n_total;i++) {
    cerr << "x[" << i << "]=" << x[i] << "\n";
    cerr << "y[" << i << "]=" << y[i] << "\n";
    cerr << "z[" << i << "]=" << z[i] << "\n";
    cerr << "r[" << i << "]=" << r[i] << "\n";
  }
}

double cost_circles (void) {
  double dx,dy,sr,tmp,cost;

  cost = 0.0;
  for (int i=0; i<n_total; i++) {
    for (int j=0; j<n_total; j++) {
      if (adj_matrix[i][j]) {
	dx = x[j] - x[i];
	dy = y[j] - y[i];
	sr = r[j] + r[i];
	
	tmp = dx*dx + dy*dy - sr*sr;
	cost += tmp*tmp;
      }
    }
  }
  return cost;
}

double cost_triangles (void) {
  double dx,dy,dz,tmp,cost;
  int i,j;

  cost = 0.0;
  for (i=0; i<n_total; i++) {
    for (j=0; j<n_total; j++) {
      if (adj_matrix[i][j]) {
	dx = x[j] - x[i];
	dy = y[j] - y[i];
	dz = z[j] - z[i];
	
	tmp = dx*dx + dy*dy + dz*dz - 1;
	cost += tmp*tmp;
      }
    }
  }
  return cost;
}

double optimize (costfunction c) {
  double tmp_cost,old_cost;
  double cost,step;

  cost = c();
  old_cost = cost + 1;
  
  while (cost<old_cost) {
    cerr << cost << "    \r";
    old_cost = cost;    

    for (int i=0; i<n_total; i++) {
      for (int j=0; j<4; j++) {
	step = sqrt(cost)/20;
	tmp_cost = cost;

	coo[j][i] += step;
	update_lattice(i);
	cost = c();

	if (cost < tmp_cost) {
	  tmp_cost = cost;
	} else {
	  coo[j][i] -= 2*step;
	  update_lattice(i);
	  cost = c();

	  if (cost < tmp_cost) {
	    tmp_cost = cost;
	  } else {
	    coo[j][i] += step;
	    update_lattice(i);
	    tmp_cost = cost;
	  }
	}
      }
    }
  }
  return cost;
}

int main (void) {
#ifdef HAVE_GMPXX
  mpf_set_default_prec (nb_bits);

  for (int i=0;i<4;i++) {
    for (int j=0;j<n_total;j++) {
      coo[i][j].set_prec(nb_bits);
    }
    per[i][0].set_prec(nb_bits);
    per[i][1].set_prec(nb_bits);
  }
#endif
  
  init_lattice();
  init_coo();

  update_lattice();

  double opt_cost = optimize (cost_triangles);

  // Calcul du module
  
  double p1x (x[2] - x[0]);
  double p2x (x[6] - x[0]);
  double p1y (y[2] - y[0]);
  double p2y (y[6] - y[0]);
  double p1z (z[2] - z[0]);
  double p2z (z[6] - z[0]);

  double n1 (sqrt(p1x*p1x + p1y*p1y + p1z*p1z));
  
  p1x = p1x / n1; 
  p1y = p1y / n1; 
  p1z = p1z / n1;
  
  p2x = p2x / n1;
  p2y = p2y / n1;
  p2z = p2z / n1;
  
  double re_tau (p1x*p2x + p1y*p2y + p1z*p2z);
  
  p2x = p2x - re_tau * p1x;
  p2y = p2y - re_tau * p1y;
  p2z = p2z - re_tau * p1z;
  
  double im_tau (sqrt(p2x*p2x + p2y*p2y + p2z*p2z));

  cerr << "\n";

  dump_coo();

  cout.precision(100);
  cout << "Module : partie réelle :\n  " << re_tau << "\n";
  cout << "         partie imaginaire :\n  " << im_tau << "\n";
  cout << "Erreur sur la fonction de coût :\n  " << opt_cost << "\n";
}
