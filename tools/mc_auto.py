#!/usr/bin/env python

import random
import vb

from subprocess import *
from numpy import *
from scipy.optimize import leastsq

def run (n):
    n = str(int(n))
    cmd = ["build/2D/Perco-box", "-n", n, "-m", n, "-p", ".6", "-q", ".4", "-t", "100000"]

    P = Popen (cmd, stdout=PIPE, stderr=open("/dev/null","w"))
    return P.communicate()[0]

D = vb.mc_data("amc.data")

fp = lambda v, x: v[0]+v[1]*(x**v[2])
e = lambda v, x, y, z: (fp(v,x)-y)/z
v = array([1., 1., -1.])

while True:
    n = array ([D.data[key][0] for key in D.data])
    x = array ([float(key.split()[0]) for key in D.data])
    y = array ([D.data[key][1][0] for key in D.data]) / n
    z = sqrt (y*(1.0-y)/n)

    v, success = leastsq (e, v, args=(x,y,z))

    xx = y - fp(v,x)
    above = xx+2*z
    below = xx-2*z

    sqerr = sum(i*i for i in above) + sum(i*i for i in below)

    m1 = list(above).index(max(above))
    m2 = list(below).index(min(below))

    if above[m1]+below[m2] > 0:
        m = m1
    else:
        m = m2

    print str(v) + " " + str(sqrt(sqerr/(2*len(x)))) + " -> " + str(x[m])

    D.parse (run(x[m]))
    D.save()
