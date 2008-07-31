#!/usr/bin/env python
# coding=iso-8859-1

"""Utility classes to go with libvb."""

__version__ = "0.1"
__revision__ = "2"
__author__ = "Vincent Beffara <vbeffara@ens-lyon.fr>"
__copyright__ = "(c) 2008 VB. GNU GPL v3."

from numpy import *
from scipy.optimize import leastsq
import sys, os

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
        (key,n,v) = l.strip().split (" | ", 2)

        n = int(n)
        vs = [float(s.split(" ")[0]) for s in v.split(" | ")]

        if key not in self.data:
            self.data[key] = [ 0, [0 for i in vs] ]

        self.data[key][0] += n
        for i in range(len(vs)):
            self.data[key][1][i] += n*vs[i]

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

        for key in sorted(self.data, key = lambda s: float(s.split()[0])):
            n = self.data[key][0]
            v = []
            for x in self.data[key][1]:
                v.append ("%f %f" % (x/n, sqrt(x*(1-x/n))/n))
            f.write ("%s | %d | %s\n" % (key,n," | ".join(v)))

    def save (self):
        if self.file:
            self.dump (self.file)
            self.dirty = False

class mc_auto (mc_data):
    def __init__ (self, run, f, v, file="amc.data"):
        mc_data.__init__ (self,file)
        self.run = run
        self.f = f
        self.v = array(v)

        self.n = array ([self.data[key][0] for key in self.data])
        self.x = array ([float(key.split()[0]) for key in self.data])
        self.y = array ([self.data[key][1][0] for key in self.data]) / self.n
        self.z = sqrt (self.y*(1.0-self.y)/self.n)

        self.xx = 0
        self.above = 0
        self.below = 0

    def e (self, v):
        ff = map (self.f, [v for i in self.x], self.x)
        return [(ff[i]-self.y[i])/self.z[i] for i in range(len(self.x))]

    def fit (self):
        self.v, success = leastsq (self.e, self.v)

        self.xx = self.y - self.f (self.v, self.x)
        self.above = self.xx + 2*self.z
        self.below = self.xx - 2*self.z

        return success

    def run_once (self, key):
        self.parse (self.run (key))
        self.save()

        i = self.data.keys().index(key)

        self.n[i] = self.data[key][0]
        self.y[i] = self.data[key][1][0] / self.n[i]
        self.z[i] = sqrt (self.y[i] * (1.0-self.y[i]) / self.n[i])

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
            self.fit()
            m = self.next()
            print self.v, self.err(), "=>", m
            self.run_once (m)

