#include <stdio.h>

int main (int argc, char **argv)
{
  int i,imax;
  if (argc==1) return 1;

  imax = atoi(argv[1]);
  if (imax<=0) return 0;

  for (i=1;i<=imax;i++) {
    printf ("%d ",i);
  }
  printf ("\n");
}
