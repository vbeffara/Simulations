#!/usr/bin/env gnuplot

FIT_LIMIT = 1e-30

fit a1*x/48+b1 'data' using (log($1)):(log($2)):(3/sqrt($6)/sqrt($2)) via a1,b1
fit a2*x/48+b2 'data' using (log($1)):(log($3)):(3/sqrt($6)/sqrt($3)) via a2,b2
fit a3*x/48+b3 'data' using (log($1)):(log($4)):(3/sqrt($6)/sqrt($4)) via a3,b3
fit a4*x/48+b4 'data' using (log($1)):(log($5)):(3/sqrt($6)/sqrt($5)) via a4,b4

set terminal x11 # persist
#set nokey

plot 'data' using (log($1)):(log($2)):(3/sqrt($6)/sqrt($2)) notitle w yer, a1*x/48+b1 t "1 arm", \
     'data' using (log($1)):(log($3)):(3/sqrt($6)/sqrt($3)) notitle w yer, a2*x/48+b2 t "2 arms", \
     'data' using (log($1)):(log($4)):(3/sqrt($6)/sqrt($4)) notitle w yer, a3*x/48+b3 t "3 arms", \
     'data' using (log($1)):(log($5)):(3/sqrt($6)/sqrt($5)) notitle w yer, a4*x/48+b4 t "4 arms"

pause -1

set terminal postscript color solid
set output 'fit.eps'

plot 'data' using (log($1)):(log($2)):(3/sqrt($6)/sqrt($2)) notitle w yer, a1*x/48+b1 t "1 arm", \
     'data' using (log($1)):(log($3)):(3/sqrt($6)/sqrt($3)) notitle w yer, a2*x/48+b2 t "2 arms", \
     'data' using (log($1)):(log($4)):(3/sqrt($6)/sqrt($4)) notitle w yer, a3*x/48+b3 t "3 arms", \
     'data' using (log($1)):(log($5)):(3/sqrt($6)/sqrt($5)) notitle w yer, a4*x/48+b4 t "4 arms"

print a1, a2, a3, a4
print sqrt(-a1/4+1),sqrt(-a2/4+1),sqrt(-a3/4+1),sqrt(-a4/4+1)
