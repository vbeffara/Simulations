#! /usr/bin/env python

import sys

pp1 = {}
pp2 = {}
pp3 = {}
pp4 = {}
kk = {}

for line in sys.stdin:
    (n, p1, p2, p3, p4, k) = line.strip().split(' ')

    n=int(n)
    k=int(k)

    p1=float(p1)
    p2=float(p2)
    p3=float(p3)
    p4=float(p4)
    
    try:
        pp1[n] += k*p1
    except KeyError:
        pp1[n] = k*p1

    try:
        pp2[n] += k*p2
    except KeyError:
        pp2[n] = k*p2

    try:
        pp3[n] += k*p3
    except KeyError:
        pp3[n] = k*p3

    try:
        pp4[n] += k*p4
    except KeyError:
        pp4[n] = k*p4

    try:
        kk[n] += k
    except KeyError:
        kk[n] = k

for n in kk.keys():
    print n, pp1[n]/kk[n], pp2[n]/kk[n], pp3[n]/kk[n], pp4[n]/kk[n], kk[n]
