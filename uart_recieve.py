import serial
import numpy as np
import matplotlib.pyplot as plt
import math as ma
import time
def byte_zusamensetzen(rx_data):
    global int_16
    
    int_16 = rx_data[0] * 256
    int_16 += rx_data[1]
    if int_16 not in adc_mittelwerte: 
        adc_mittelwerte.append(int_16*3300/32768)

    #print(rx_data)
    #print(rx_data[0])
    #print(rx_data[1])
    #print(adc_mittelwerte)
    

if __name__ == "__main__":

    uart = serial.Serial('COM4', baudrate= 9600, inter_byte_timeout=10) # 1 Stopbit, kein Parity

    adc_mittelwerte = []
    counter_mittelwerte = 0
    int_16 = 0
    msb_bool = True

    N = 2**8

    while len(adc_mittelwerte) < N:
        rx_data = uart.read(2)
        if not (rx_data == b''):
            byte_zusamensetzen(rx_data)

        print(len(adc_mittelwerte))
            #time.sleep()

    x = np.arange(0,N,1)
    plt.scatter(x,adc_mittelwerte, s=0.5, vmin=0, vmax=3300) # marker = 'o'
    plt.xlabel("Eingang des R2R Netzwerkes")
    plt.ylabel("Ausgangsspannung des R2R Netzwerkes in mV")
    plt.axis([0,256,0,3300])
    plt.show()
    
    
