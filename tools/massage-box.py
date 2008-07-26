#! /usr/bin/env python

import sys

g = {}
n = {}

for line in sys.stdin:
    (n1, n2, p, q, t, tg, x) = line.strip().split(' ')
    t = int(t)
    x = float(x)

    key = ' '.join((n1,n2,p,q))

    if not key in n:
        n[key] = 0
        g[key] = 0

    n[key] += t
    g[key] += x*t

for key in n:
    print key, n[key], g[key]/n[key]
