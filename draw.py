#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import numpy as np

def F(x, a):
    e = np.exp(a[1] * x)
    return a[0] * N0 * e / (a[0] + N0 * (e - 1))

def F_by_a0(x, a):
    e = np.exp(a[1] * x)
    z = a[0] + N0 * (e - 1)
    return N0 * e / z - a[0] * N0 * e / (z * z)

def F_by_a1(x, a):
    e = np.exp(a[1] * x)
    z = a[0] + N0 * (e - 1)
    return a[0] * a[1] * N0 * e / z - a[0] * a[1] * N0 * N0 * e * e / (z * z)

def f1(a):
    s1 = 0
    for xj, yj in zip(x, y):
        d = F(xj, a) - yj
        s1 += d * F_by_a0(xj, a)
    return 2 * s1

def f2(a):
    s2 = 0
    for xj, yj in zip(x, y):
        d = F(xj, a) - yj
        s2 += d * F_by_a1(xj, a)
    return 2 * s2

a = np.array([1, -7])
aNext = np.array([0, 0])
x, y = np.genfromtxt("text.txt", unpack=True)
xMax = np.max(x)
x = np.divide(x, xMax / 2)
x = np.subtract(x, 1)
# N0 = np.max(y)
y = np.divide(y, np.max(y))
N0 = 0.253
h = 0.01
h0 = np.array([h, 0])
h1 = np.array([0, h])
it = 0
eps = 1e-4
n = 1
while n >= eps:
    Af = np.array([[(f1(np.add(h0, a)) - f1(a)) / h, (f1(np.add(h1, a)) - f1(a)) / h], [(f2(np.add(h0, a)) - f2(a)) / h, (f2(np.add(h1, a)) - f2(a)) / h]])
    r = np.dot(np.linalg.inv(Af), np.array([f1(a), f2(a)]))
    aNext = np.subtract(a, r)
    it += 1
    n = np.linalg.norm(np.subtract(a, aNext))
    print(it, ": ", aNext, ": ", n)
    a = aNext
print('Итоговые параметры решения:')
print('K = ', a[0])
print('r = ', a[1])

def plotfun(t):
    e = np.exp(a[1] * t)
    return a[0] * N0 * e / (a[0] + N0 * (e - 1))

absEr = []
for xj, yj in zip(x, y):
    absEr.append(np.abs(yj - plotfun(xj)))

print('Ошибка не превышает: ', np.max(absEr), ' %')

import matplotlib.pyplot as plt
fig = plt.figure()
plt.plot(x, np.array(list(map(plotfun, x))), label=u'Аналитическое решение', color='coral')
plt.plot(x, y, label=u'Модель', color='navy')
plt.grid(True)
plt.legend()
plt.title(u'Динамика популяции')
plt.show()
