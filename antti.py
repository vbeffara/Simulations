#!/usr/local/bin/python3
# coding: utf-8

import numpy as np
from math import log, inf

N = 10000
p = 2*log(N)/N

A = np.zeros((N, N))
for i in range(N):
    for j in range(i+1, N):
        if (np.random.uniform() < p):
            A[i, j] = 1
            A[j, i] = 1

l, vs = np.linalg.eigh(A)

for i in range(N):
    print(l[i], np.linalg.norm(vs[:, i], ord=inf))
