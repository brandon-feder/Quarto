import numpy as np
import os

a = ": > debug.out;"
b = "cd {}/bot;".format(os.getcwd())
c = "g++ main.cpp -O3 -ljsoncpp; ./a.out; rm ./a.out;"

os.system(a + b + c)
