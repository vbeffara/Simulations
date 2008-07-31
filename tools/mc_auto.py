#!/usr/bin/env python

from subprocess import *
from math import sqrt
import vb

def run (key):
    n = key.split()[0]
    cmd = ["build/2D/Perco-box", "-n", n, "-m", n,
            "-p", ".6", "-q", ".4", "-t", "100000"]

    P = Popen (cmd, stdout=PIPE, stderr=open("/dev/null","w"))
    return P.communicate()[0]

f = lambda v, x: v[0]+v[1]*(x**v[2])

F = vb.mc_auto (run, f, [1.,1.,-1.])
F.loop()
