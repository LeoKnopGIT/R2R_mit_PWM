#include "msp430F5529.h"
#include "lib/init.h"

void initSPI(void);
//__interrupt void USCI_B0_ISR(void);

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
    initSPI();
    initMC();
    __bis_SR_register(GIE);                   // Aktiviert allgemeine Interrupts
    
    while(1);
}
// SPI Interrupt Service Routine
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
    
    UCB0TXBUF = 0x5F;
                                     
    UCB0IFG &= ~UCTXIE;
}
// PORT Interrupt Service Routine
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  UCB0TXBUF = 0xF5;
}
/*
#include "lib/init.h"
#include "lib/interface.h"

#define pwm_aufloesung 8
#define filter_aufloesung 8
#define r2r_aufloesung 8

// Globale Variablen Deklaration
unsigned int conversioncounter = 0;     // Zeigt an wie viele Werte der ADC schon übertragen hat
unsigned int pwm_on = 0;                // Pulsbreitenverhältniss: pwm_on/2^pwm_aufloesung
unsigned int pwm_anzahl = (1 << pwm_aufloesung); // Auflösung PWM (einstellbar durch k)

unsigned int r2r_ansteuerung = 0;       // legt fest welche Bits des R2R Netzwerkes gesetzt werden
unsigned int r2r_anzahl = (1 << r2r_aufloesung);

unsigned int mittelwert = 0;
unsigned int mittelwert_anzahl = (1 << filter_aufloesung);
long sum;                               // Summe zum berechnen des Mittelwerts ( muss double sein, da 8Bit * 16 Bit = 28 BIT Zahlen Adiert werden)

char rx_data = 0;

void main(void)
{
  // Initialisierungen
  initMC();
  initPWM(pwm_anzahl, pwm_on);
  initR2R();
  initSPI();
  initUART();
  
  __bis_SR_register(GIE);     // Global Interrupt Enable
  
  while(1)
  {
    P4OUT |= BIT7;                      // Grüne LED leuchtet wenn das Programm bereit zum Starten ist
    P1OUT &= ~BIT0;                     // RoteLED aus
    while(0 == P1IFG);                  // Wenn S2 gedrückt wird, startet das Programm
    P1OUT |= BIT0;                      // Rote LED leuchtet wenn das Programm läuft
    P4OUT &= ~BIT7;                     // Grüne LED aus
    
    r2r_ansteuerung = 0;                // Zum Start alle "Bits" des R2R auf 0
    while(r2r_ansteuerung < r2r_aufloesung)                                      // Schleife steuert jede Biteinstellung des R2R-Netzwerkes an
    {
      P6OUT = r2r_ansteuerung;
      pwm_on = 0;
      while(pwm_on < pwm_anzahl)                                                // Schleife steuert jedes Pulsbreitenverhältniss an
      {
        mittelwert = 0;
        sum = 0;
        conversioncounter = 0;
        while(conversioncounter < mittelwert_anzahl)                            // Schleife bildet Mittelwert
        {
          // Wait for SPI Interrupts
          sum = sum + SPI_recieve();    // Messung durch ADC
        }
        mittelwert = sum >> filter_aufloesung;          // Teilen der Summe durch 256
        UART_send(mittelwert);                          // Über UART an PC
        ++pwm_on;                                       // erhöht Pulsbreite und fungiert als counter
      }
      ++r2r_ansteuerung;
    }
    UCB0TXBUF = 0xF;
    while((UCRXIFG & UCB0IFG) == 0);
    if(!rx_data == 0) 
    {
      P4OUT |= BIT7;                      
      P1OUT &= ~BIT0;
    } else 
    {
      P4OUT &= ~BIT7;                      
      P1OUT |= BIT0;
    }
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#pragma vector = USCI_B0_VECTOR
__interrupt void ISR_USCI_B0(void)
{
  rx_data = UCB0RXBUF;                  // Auslesen löscht flag
}
*/