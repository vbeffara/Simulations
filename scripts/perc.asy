int xdim = 40;
int ydim = 30;
real p = .7;

import stats;

unitsize (5);

for (int i=0; i<=xdim; ++i)
  for (int j=0; j<=ydim; ++j)
    dot ((i,j), linewidth(.8));

for (int i=0; i<xdim; ++i)
  for (int j=0; j<=ydim; ++j)
    if (unitrand() < p) draw ((i,j)--(i+1,j));
//    else draw ((i+.5,j-.5)--(i+.5,j+.5), dotted);

for (int i=0; i<=xdim; ++i)
  for (int j=0; j<ydim; ++j)
    if (unitrand() < p) draw ((i,j)--(i,j+1));
//    else draw ((i-.5,j+.5)--(i+.5,j+.5), dotted);
