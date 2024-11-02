import numpy as np
from sklearn.linear_model import LinearRegression
import matplotlib.pyplot as plt
import pathlib

def calc(p,N):
    x = np.arange(0,N,1)
    x_1 = np.arange(0,N-1,1)
    
    
    y = np.loadtxt(p)

    offset_korr = y[0]                          # y-Achsenabschnitt
    m_ptp = (y[N - 1] - y[0])/N                 # Steigung PtP-Gerade
    m_ideal = 3300*(N-1)/N/N                    # FS-Ausgangsspannung/N 
    gain_korr = m_ideal/m_ptp 
    y_korr = (y - offset_korr) * gain_korr      # Korrektur 
    u_lsb = 3300/(N)                        # 1 LSB fuer dnl/inl
    
    y_ideal = x * u_lsb
    
    i = 0
    dnl = []
    while i < N - 1:
        dnl.append(((y_korr[i + 1] - y_korr[i]) / u_lsb) - 1)
        i += 1

    i = 0
    inl = []
    inl_sum = 0
    while i < N - 1:
        inl_sum = inl_sum + dnl[i]
        inl.append(inl_sum)
        i += 1

    y_ideal = x * u_lsb
    diff = y_ideal - y
    i = 0
    u_eff_list = []

    while i < len(y):
        u_eff_list.append(diff[i]*diff[i])
        i += 1
    u_eff = np.sqrt(np.sum(u_eff_list)/len(u_eff_list))
    ENOB = np.log2(3300*(N-1)/N/u_eff)
    
    print("offset",offset_korr)
    print("gain",gain_korr)
    print("ENOB", ENOB)
    print("max INL",max(inl),min(inl))
    print("max DNL",max(dnl),min(dnl))
    return dnl, inl, y_korr, x, x_1, y

if __name__ == "__main__":

    
    p = pathlib.PurePath('Ergebnisse','14BitMitKorr2.txt')
    
    n = 14
    N = 2**n # 8 + pwm auflösung
    dnl, inl, y_korr, x, x_1, y = calc(p,N)

    plt.figure()
    plt.subplots_adjust(left=0.12, bottom=0.12, right=0.99, top=0.99)
    plt.scatter(x_1, dnl, s=0.5)
    plt.xlabel('Digitalwert')
    plt.ylabel('DNL in LSB')
    plt.grid(True)

    plt.figure()
    plt.subplots_adjust(left=0.12, bottom=0.12, right=0.99, top=0.99)
    plt.scatter(x_1, inl, s=0.5)
    plt.xlabel('Digitalwert')
    plt.ylabel('INL in LSB')
    plt.grid(True)
    plt.show()


'''
    p_speichern = pathlib.PurePath('Ergebnisse','R2R_DNL_für_korr.txt'
    dnl_korr = []
    for i in dnl:
        dnl_korr.append(i[0]*16)
        
    #np.savetxt(p_speichern, dnl_korr, fmt='%i', delimiter=",",newline=',')
'''


