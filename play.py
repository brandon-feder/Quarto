import numpy as np
import os

a = "cd {}/bot;".format(os.getcwd())
b = "g++ main.cpp -O3; ./a.out; rm ./a.out;"

os.system(a + b)

# def p(M):
#     print(str(M.reshape(16)).replace("[", "").replace("]", "").replace(" ", ","))

# mat = np.array([0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 , 11, 12, 13, 14, 15 ]).reshape(4, 4)
#
# A = np.rot90(mat)
# B = np.flip(A)
# C = np.rot90(A)
# D = np.flip(C)
# E = np.rot90(C)
# F = np.flip(E)
# G = np.rot90(E)
#
# p(mat)
# p(A)
# p(B)
# p(C)
# p(D)
# p(E)
# p(F)
# p(G)
