#!/usr/bin/env python
# coding=iso-8859-1

"""Utility classes to go with libvb."""

__version__ = "0.1"
__revision__ = "2"
__author__ = "Vincent Beffara <vbeffara@ens-lyon.fr>"
__copyright__ = "(c) 2008 VB. GNU GPL v3."

import sys, os

from numpy import *
from scipy.optimize import leastsq

class mc_data (object):
    def __init__ (self, file=None):
        self.data = {}
        self.comments = []
        self.dirty = False
        self.file = file

        if self.file:
            if not os.path.isfile (self.file):
                open (self.file, "w")
            self.read (self.file)

    def __del__ (self):
        if self.dirty:
            self.save()

    def parse (self, l):
        (k,n,v) = l.strip().split (" | ", 2)

        n = int(n)
        key = tuple(float(s) for s in k.strip(" ").split(" "))
        vs = [int(s.strip(" ").split(" ")[0]) for s in v.split(" | ")]

        if key not in self.data:
            self.data[key] = [ 0, [0 for i in vs] ]

        self.data[key][0] += n
        for i in range(len(vs)):
            self.data[key][1][i] += vs[i]

        self.dirty = True
        return key

    def read (self, file):
        for l in open(file):
            if l[0] == "#":
                self.comments.append (l)
            else:
                self.parse (l)

    def dump (self, file=None):
        if file:
            f = open (file,"w")
        else:
            f = sys.stdout

        for l in self.comments:
            f.write (l)

        for key in sorted(self.data):
            n = self.data[key][0]
            k = " ".join(str(i) for i in key)
            v = []
            for x in self.data[key][1]:
                r = float(x)/n
                v.append ("%d %f %f" % (x, r, sqrt(r*(1-r)/n)))
            f.write ("%s | %d | %s\n" % (k,n," | ".join(v)))

    def save (self):
        if self.file:
            self.dump (self.file)
            self.dirty = False

class mc_auto (mc_data):
    def __init__ (self, run, f, v, file="amc.data"):
        mc_data.__init__ (self,file)
        self.run = run
        self.f = f
        self.v = v

        self.n = array ([self.data[key][0] for key in self.data])
        self.x = array (self.data.keys())
        self.y = array ([float(self.data[key][1][0]) for key in self.data]) / self.n
        self.z = sqrt (self.y*(1.0-self.y)/self.n)

        self.fx = 0
        self.above = 0
        self.below = 0

        self.fit()

    def e (self, v):
        ff = array ([self.f (v,x) for x in self.x])
        return (ff-self.y)/self.z

    def fit (self):
        self.v, success = leastsq (self.e, self.v)

        self.fx = array ([self.f (self.v,x) for x in self.x])
        self.above = self.fx - self.y + 2*self.z
        self.below = self.fx - self.y - 2*self.z

        return success

    def run_once (self, key):
        self.parse (self.run (key))
        self.save()

        i = self.data.keys().index(key)

        self.n[i] = self.data[key][0]
        self.y[i] = float(self.data[key][1][0]) / self.n[i]
        self.z[i] = sqrt (self.y[i] * (1.0-self.y[i]) / self.n[i])

        self.fit()

    def err (self):
        sqerr = sum(i*i for i in self.above) + sum(i*i for i in self.below)

        return sqrt (0.5 * sqerr / len(self.x))

    def next (self):
        if self.above.max() + self.below.min() > 0:
            m = self.above.argmax()
        else:
            m = self.below.argmin()

        return self.data.keys()[m]

    def loop (self):
        while True:
            m = self.next()
            #print self.v, self.err(), "=>", " ".join(str(i) for i in m)
            self.run_once (m)
