/*
 * Rarefaction - TASEP Rarefaction fan
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <unistd.h>

#include <vb/PRNG.h>

long n;

int main (int argc, char **argv) {
  int *field;
  int i,t,running;
  char ch;

  n = 20;

  while ((ch = getopt(argc,argv,"hn:")) != -1) {
    switch (ch) {
    case 'n':
      n = atoi (optarg);
      break;
    case 'h':
      fprintf (stderr, "Syntax: %s [-n size]\n", argv[0]);
      exit (1);
      break;
    }
  }

  field = (int*) calloc (2*n,sizeof(int));

  for (i=0;i<2*n;i++) field[i]=n-i;

  running=1;
  while (running) {
    i = vb::prng()%(2*n-1);
    if (field[i]>field[i+1]) {
      t = field[i];
      field[i] = field[i+1];
      field[i+1] = t;
    }
    if ((field[0]<0) || (field[2*n-1]>0))
      running=0;
  }

  for (i=0;i<2*n;i++) {
    printf ("%d %d\n", i, field[i]);
  }

  free(field);

  return 0;
}
