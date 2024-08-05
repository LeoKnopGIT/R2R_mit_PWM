## Aufbau der Hardware:
![Schaltplan](https://github.com/LeoKnopGIT/R2R_mit_PWM/blob/main/Schaltplan%20Hybrid%20DAC%20R2R_PWM.png)

**Momentane Funktion:**
- Bei Betätigen von TasterS2 gibt der MC ein CLK-Signal (für 2 byte) an den ADC + CS wird auf LOW gezogen
    
 **TO DO:**
 - SPI-Schnittstelle realisieren (Signal auf Oszi sichtbar, nun auslesen von der Antwort des ADCs)

 - Code funktionalität SPI-Interrupt schreiben -> main sollte möglichst frei sein

 - Python script zum empfangen uind auswerten der Messdaten