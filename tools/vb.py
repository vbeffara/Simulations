#!/usr/bin/env python
# coding=iso-8859-1

"""Utility classes to go with libvb."""

__version__ = "0.1"
__revision__ = "2"
__author__ = "Vincent Beffara <vbeffara@ens-lyon.fr>"
__copyright__ = "(c) 2008 VB. GNU GPL v3."

from math import sqrt
import sys

class mc_data (object):
    def __init__ (self, file=None):
        self.data = {}
        self.comments = []
        self.file = file

        if self.file:
            self.read (self.file)

    def __del__ (self):
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
