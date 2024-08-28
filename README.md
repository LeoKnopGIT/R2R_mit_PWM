## Aufbau der Hardware:  

![Schaltplan](https://github.com/LeoKnopGIT/R2R_mit_PWM/blob/main/Schaltplan%20Hybrid%20DAC%20R2R_PWM.png)
    
 **TO DO:**  
 
• Converversion monitoring um Zeit zu sparen  

  -> dafür MISO an Port 3.1 mit Port 2.6 verbinden und diesen als Input configuriern. Mit dem TimerB0 Chip Select Reset und dann überprüfung ob MISO gesetzt wurde über Port 2.6  
  
• Pythonauswertung (Messdaten am besten in csv speichern)  

• Uart sollte mit den Messdaten noch einen Identifier senden  

 **Erste Ergebnisse:**  
 
 ![R2R-TEst](https://github.com/LeoKnopGIT/R2R_mit_PWM/blob/main/nurr2r.png)
