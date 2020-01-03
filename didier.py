#!/usr/local/bin/python3
# coding: utf-8

import numpy as np
import cvxpy as cp

K = 30  # nb of students,
k = 20  # Minimum size of a class
N = 6   # nb of options
n = 4   # Number of options to affect to each student

W = np.zeros((K, N), dtype=int)
for i in range(K):
    W[i, :] = np.random.permutation(range(1, 1+N))
print("Student wishes : \n%s" % W)

"""find P (matrix of size (K,N), made of 0 and 1)
   such that all lines of P sum to n
   such that all columns of P sum to at least k
   and minimizing cost(P) = max diagonal term of W P^t"""

if K*n < k*N:
    print("Not enough students to satisfy the requirement")
    exit(1)

k_ones = np.ones(K, dtype=int)
n_ones = np.ones(N, dtype=int)

x = cp.Variable((K, N), boolean=True)
objective = cp.Minimize(cp.max(cp.diag(W * x.T)))
constraints = [k_ones * x >= k, x * n_ones == n]
problem = cp.Problem(objective, constraints)
problem.solve()

P = x.value.round().astype(int)
print("Class compositions : \n%s" % P)
print("Satisfactions : %s" % W.dot(P.transpose()).diagonal())
print("Choices per student : %s" % P.dot(n_ones))
print("Class sizes : %s" % k_ones.dot(P))
