import numpy as np
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import pathlib

p = pathlib.PurePath('Ergebnisse','pwm_0__mittelwert_0.txt')

N = 2**8 # 8 + pwm auflösung

x = np.arange(0,N,1).reshape((-1, 1))
x_1 =np.arange(0,N-1,1).reshape((-1, 1))

y = np.loadtxt(p).reshape((-1, 1))

m = LinearRegression().fit(x,y)
y_fit = m.predict(x)

offset_korr = y[0]
gain_korr = (3300/N)/m.coef_ 

print(offset_korr)
print(gain_korr)
print(f'R2: {m.score(x,y)}')

y_korr = (y - offset_korr) * gain_korr 

y_ideal= (x/N)*3300

u_lsb = 3300/N

i = 0
dnl = []
while i < len(y_korr) - 1:
    dnl.append(((y_korr[i + 1] - y_korr[i]) / u_lsb) - 1)
    i += 1

i = 0
inl = []
inl_sum = 0
while i < len(y_korr) - 1:
    inl_sum = inl_sum + dnl[i]
    inl.append(inl_sum)
    i += 1


plt.figure()
plt.scatter(x_1, dnl, s=0.4)
plt.xlabel('Digitalwert')
plt.ylabel('DNL')
plt.grid(True)

plt.figure()
plt.scatter(x_1, inl, s=0.4)
plt.xlabel('Digitalwert')
plt.ylabel('INL')
plt.grid(True)








