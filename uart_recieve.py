import serial
import numpy as np
import matplotlib.pyplot as plt
import math as ma

def byte_zusamensetzen(byte):
    global msb_bool
    global int_16
    if msb_bool:
        int_16 = int.from_bytes(byte) * 256
        msb_bool = False
    else:
        int_16 += int.from_bytes(byte)
        
        adc_mittelwerte.append(int_16)
        msb_bool = True

if __name__ == "__main__":

    uart = serial.Serial('COM4', baudrate= 9600, timeout= 1) # 1 Stopbit, kein Parity

    adc_mittelwerte = []
    int_16 = 0
    msb_bool = True

    while len(adc_mittelwerte) < 1:
        rx_data = uart.read()
        if not (rx_data == b''):
            byte_zusamensetzen(rx_data)

            

    print(adc_mittelwerte)

    x = np.arange(0,1,2^15)
    
    
