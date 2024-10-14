import numpy as np
import matplotlib.pyplot as plt
import pathlib

p = pathlib.PurePath('Ergebnisse','pwm_0__mittelwert_0.txt')

N = 2**8 # 8 + pwm auflösung

x = np.arange(0,N,1)
analog = (x/N)*3300

f = np.loadtxt(p)

i = 0
dnl = []
dnl.append(0.00)
while i < len(f):
    if i > 0:
        dnl.append(((f[i] - f[i-1]) / (3300/(N - 1))) - 1)
    i += 1

i = 0
inl = []
while i < len(x):
    inl.append((f[i]-(x[i]/(N-1))*3300) / (3300/(N - 1)))

    #inl.append((f[i] - (x[i]/(N - 1))*(N - 1)))
    i += 1

diff = analog - f

sum = 0
for i in diff:
    sum = i + sum
    avr_offset = sum / N

sum = 0
for i in diff:
    sum = i * i + sum
    avr_square_offset = sum / N

rms = avr_square_offset ** 0.5

print("avr_offset in mv: ",avr_offset)
print("avr_square_offset in mv: ", rms)

plt.figure()
plt.scatter(x, inl, s=0.4)
plt.ylabel('INL')
plt.grid(True)
plt.show

plt.figure()
plt.scatter(x, dnl, s=0.4)
plt.ylabel('DNL')
plt.grid(True)
plt.show

plt.figure()
plt.scatter(x , diff, s=0.4) # marker = 'o'
plt.ylabel("Ausgangsspannung des DAC in mV")
plt.grid(True)
plt.show()
