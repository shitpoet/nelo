import math
import json

import numpy as np
from scipy.optimize import curve_fit
from matplotlib import pyplot as plt

obj = json.load(open('play-results.json'))
nodes = []
elos = []
for (k, v) in obj.items():
    nodes.append(int(k))
    elos.append(float(v.split('__')[0].split('_')[0]))

print('nodes')
print(nodes)
print('elos')
print(elos)


log_nodes = [math.log2(x) for x in nodes]
print('log_nodes')
print(log_nodes)

# numpy.linspace with the given arguments
# produce an array of 40 numbers between 0
# and 10, both inclusive
x = np.array(log_nodes)
#print('len(x)')
#print(len(log_nodes))


# y is another array which stores 3.45 times
# the sine of (values in x) * 1.334.
# The random.normal() draws random sample
# from normal (Gaussian) distribution to make
# them scatter across the base line
y = np.array(elos)

# Test function with coefficients as parameters
def test(x, a, b, c):
    return a * x ** 2 + b * x + c

# curve_fit() function takes the test-function
# x-data and y-data as argument and returns
# the coefficients a and b in param and
# the estimated covariance of param in param_cov
param, param_cov = curve_fit(test, x, y)

print('function coefficients:')
print(param)
print('covariance of coefficients:')
print(param_cov)

# ans stores the new y-data according to
# the coefficients given by curve-fit() function
ans = []
for xx in log_nodes:
    ans.append( test(xx, *param) )
ans = np.array(ans)
print('ans = f(x)')
print(ans)

#'''Below 4 lines can be un-commented for plotting results
#using matplotlib as shown in the first example. '''
#
#plt.plot(x, y, 'o', color ='red', label ="data")
#plt.plot(x, ans, '--', color ='blue', label ="optimized data")
#plt.legend()
#plt.show()

def elo_to_nodes(elo):
    # elo == a * log2(nodes) ** 2 + b * log2(nodes) + c
    # t == log2(nodes)
    # nodes == 2 ** t
    # elo == a * t ** 2 + b * t + c
    a, b, c = [ -2.69406196,  209.11966381, -230.07201856 - elo ]
    det = b ** 2 - 4 * a * c
    if det >= 0:
        t1 = (-b + math.sqrt(det)) / (2 * a)
        t2 = (-b - math.sqrt(det)) / (2 * a)
        n1 = 2 ** t1
        n2 = 2 ** t2
        if 0 <= n1 <= 600000:
            return n1
        elif 0 <= n2 <= 600000:
            return n2
    else:
        print('det < 0')
    return math.nan

print()
import random
params = [ -2.69406196,  209.11966381, -230.07201856]
for _ in range(5):
    n = random.random() * 1000
    res_elo = test(math.log2(n), *params)
    print('nodes', n, 'elo', res_elo)
    nn = elo_to_nodes(res_elo)
    print('nodes*', nn)
print()
for e in range(50, 2700 + 50, 50):
    print('elo', e, 'nodes*', (elo_to_nodes(e)))
