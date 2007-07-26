#!/usr/bin/env gnuplot

FIT_LIMIT = 1e-30

fit a1*x/48+b1 'data' using (log($1)):(log($2)):(3/sqrt($6)) via a1,b1
fit a2*x/48+b2 'data' using (log($1)):(log($3)):(3/sqrt($6)) via a2,b2
fit a3*x/48+b3 'data' using (log($1)):(log($4)):(3/sqrt($6)) via a3,b3
#fit a4*x/48+b4 'data' using (log($1)):(log($5)) via a4,b4

set terminal x11 persist
set nokey

plot 'data' using (log($1)):(log($2)):(3/sqrt($6)) t "1 arm" w yer, a1*x/48+b1, \
     'data' using (log($1)):(log($3)):(3/sqrt($6)) t "2 arms" w yer, a2*x/48+b2, \
     'data' using (log($1)):(log($4)):(3/sqrt($6)) t "3 arms" w yer, a3*x/48+b3

set terminal postscript color solid
set output 'fit.eps'

plot 'data' using (log($1)):(log($2)):(3/sqrt($6)) t "1 arm" w yer, a1*x/48+b1, \
     'data' using (log($1)):(log($3)):(3/sqrt($6)) t "2 arms" w yer, a2*x/48+b2, \
     'data' using (log($1)):(log($4)):(3/sqrt($6)) t "3 arms" w yer, a3*x/48+b3

print a1, a2, a3
print sqrt(-a1/4+1),sqrt(-a2/4+1),sqrt(-a3/4+1)
