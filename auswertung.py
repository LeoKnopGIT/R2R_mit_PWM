import numpy as np
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import pathlib

def calc(p,N):
    x = np.arange(0,N,1).reshape((-1, 1))
    x_1 = np.arange(0,N-1,1).reshape((-1, 1))

    y = np.loadtxt(p).reshape((-1, 1))

    m = LinearRegression().fit(x,y)
    y_fit = m.predict(x)

    offset_korr = y_fit[0]
    gain_korr = (3300/N)/m.coef_ 

    print(offset_korr)
    print(m.coef_)
    print(gain_korr)
    print(f'R2: {m.score(x,y)}')

    y_ideal = x*3300/N

    y_korr = (y - offset_korr) * gain_korr 
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

    i = 0
    avg_offset_error = 0
    while i < len(y):
        avg_offset_error = (y_ideal[i] - y[i]) + avg_offset_error
        i = i + 1
    avg_offset_error =  avg_offset_error/N
    print("offseterror:",avg_offset_error)

    return dnl, inl, y_korr, y_fit, x, x_1, y

if __name__ == "__main__":
    
    p = pathlib.PurePath('Ergebnisse','pwm_0__mittelwert_0.txt')
    p_speichern = pathlib.PurePath('Ergebnisse','R2R_INL_für_korr.txt')
    n = 8

    
    N = 2**n # 8 + pwm auflösung
    dnl, inl, y_korr, y_fit, x, x_1, y= calc(p,N)
    inl_korr = []
    for i in inl:
        inl_korr.append(round(i[0]*16))
        
    np.savetxt(p_speichern, inl_korr, fmt='%i', delimiter=",",newline=',')

    plt.figure()
    plt.subplots_adjust(left=0.12, bottom=0.12, right=0.99, top=0.99)
    plt.scatter(x_1, dnl, s=0.4)
    plt.xlabel('Digitalwert')
    plt.ylabel('DNL in LSB')
    plt.grid(True)

    plt.figure()
    plt.subplots_adjust(left=0.12, bottom=0.12, right=0.99, top=0.99)
    plt.scatter(x_1, inl, s=0.4)
    plt.xlabel('Digitalwert')
    plt.ylabel('INL in LSB')
    plt.grid(True)

    plt.figure()
    plt.subplots_adjust(left=0.12, bottom=0.12, right=0.99, top=0.99)
    plt.scatter(x, y, s=0.4)
    plt.xlabel('Digitalwert')
    plt.ylabel('Ausgegebene Spannung in mv')
    plt.grid(True)

    plt.show()

    print("MAX_INL:", max(inl))







