import serial
import numpy as np
import matplotlib.pyplot as plt
import pathlib

def byte_zusamensetzen(rx_data):
    global counter_pc

    counter_mc = rx_data[0] * 256
    counter_mc += rx_data[1]

    ausgangswert_adc = rx_data[2] * 256
    ausgangswert_adc += rx_data[3]

    #print("counter_mc",counter_mc)
    #print("counter_pc",counter_pc)

    if counter_pc == counter_mc: 
        spannungswerte.append((ausgangswert_adc - 32768) * 3300/32768 ) # *3300/32768
        #spannungswerte.append((ausgangswert_adc - 32768) ) # *3300/32768

        digitalwerte.append(counter_mc)
        if ((ausgangswert_adc - 32768) * 3300/32768 ) > 3000:
            print("ALARM")
        counter_pc += 1
        
        
if __name__ == "__main__":

    uart = serial.Serial('COM4', baudrate= 9600) # 1 Stopbit, kein Parity

    spannungswerte = []
    digitalwerte = []
    counter_pc = 0

    N = 2**14 # 8 + pwm auflösung

    while len(spannungswerte) < N:
        rx_data = uart.read(4)
        if not (rx_data == b''):
            byte_zusamensetzen(rx_data)

        print("laenge list", len(spannungswerte))

    p = pathlib.PurePath('Ergebnisse','14BitmitKorr2.txt')
    diff = 1000 - spannungswerte

    i = 100
    u_eff_list = []

    while i < len(y):
        u_eff_list.append(diff[i]*diff[i])
        i += 1
    u_eff = np.sqrt(np.sum(u_eff_list)/len(u_eff_list))
    ENOB = np.log2(3300*(N-1)/N/u_eff)
    print(ENOB)
    np.savetxt(p,spannungswerte)

    plt.scatter(digitalwerte,spannungswerte, s=0.4) # marker = 'o'
    plt.xlabel("Eingang des R2R Netzwerkes")
    plt.ylabel("Ausgangsspannung des R2R Netzwerkes in mV")
    # plt.axis([0,N,0,3300])
    plt.grid(True)
    plt.show()
    

    
    
