import numpy as np
import matplotlib.pyplot as plt
import pathlib

pin = pathlib.PurePath('Ergebnisse','r2r_korrektur_array.txt')
pout = pathlib.PurePath('Ergebnisse','r2r_korrektur_array_formattiert.txt')
p1 = pathlib.PurePath('Ergebnisse','r2r_korrektur_3.txt')
p2 = pathlib.PurePath('Ergebnisse','r2r_korrektur_1.txt') 

N = 2**8 # 8 + pwm auflösung

x = np.arange(0,N,1)
analog = (x/N)*3300

digital = x*(2**7)

f1 = np.loadtxt(p1)
f2 = np.loadtxt(p2)

diff = digital - f1
#np.savetxt(p0,diff,fmt='%d')


plt.scatter(x , diff, s=0.4) # marker = 'o'

plt.ylabel("Ausgangswert des DAC")

plt.grid(True)

plt.show()
with open(pin, 'r') as infile, open(pout, 'w') as outfile:
        for line in infile:
            line = line.strip()  # Entfernt Zeilenumbrüche
            outfile.write(f"{line},")