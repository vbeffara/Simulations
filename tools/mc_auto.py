#!/usr/bin/env python

from subprocess import *
import vb

def run (key):
    n = str(key[0])
    cmd = ["build/2D/Perco-Arms", "-n", n, "-t", "100"]

    P = Popen (cmd, stdout=PIPE, stderr=open("/dev/null","w"))
    return P.communicate()[0]

def f (v, x):
    return v[0]*(x[0]**(v[1]/48))

F = vb.mc_auto (run, f, [1.,-5.])
F.loop()
