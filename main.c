#include "msp430F5529.h"
#include "lib/init.h"

//Einstellungen
#define pwm_aufloesung 4                                                        
#define mittelwert_aufloesung 0                                               
#define r2r_korrektur 1                                                         // 1 -> Systematischer Fehler des R2R-Netzwerkes wird korregiert, 0 -> wird nicht korregiert

// Globale Variablen Deklaration
unsigned int uart_counter = 0;
unsigned int r2r_counter = 0;

unsigned int pwm_on = 0;                                                        // Pulsbreitenverhältniss
unsigned int pwm_anzahl = (1 << pwm_aufloesung) - 1;

unsigned long mittelwert = 0;
unsigned int mittelwert_counter = 0;
unsigned int mittelwert_anzahl = (1 << mittelwert_aufloesung)- 1;
long sum = 0;     

char rx_data;
char tx_data;

unsigned int adc_value;
unsigned int msb_or_lsb;

unsigned int vorlauf = 0;                                                           // ersten beiden Werte des ADC's sind zu "löschen"

unsigned int korrektur_array[] =    {1,26,25,24,23,22,21,20,21,20,19,18,17,16,15,       //Experimentell ermittelte Fehlerkorrekturdaten
                                     14,17,16,15,14,13,12,11,9,11,10,9,8,8,6,6,4,
                                     19,21,21,19,19,17,17,15,17,16,16,14,14,12,12,
                                     11,14,12,12,10,10,6,8,7,9,8,8,6,6,4,4,2,18,20,
                                     20,18,18,16,16,15,17,15,15,13,13,11,11,10,13,
                                     11,12,10,10,8,8,6,8,6,8,5,6,4,4,2,17,19,20,17,
                                     18,15,16,14,16,14,15,13,14,11,12,8,12,10,12,9,
                                     10,7,8,6,9,6,7,4,7,2,3,1,34,39,41,38,39,37,39,
                                     35,39,36,38,34,36,33,35,32,36,34,36,32,34,31,
                                     34,30,33,31,33,30,31,28,30,27,43,43,46,42,44,
                                     41,43,39,45,41,43,39,41,38,40,36,40,38,41,37,
                                     39,36,38,34,38,32,37,34,36,32,34,30,47,48,50,
                                     47,49,45,48,44,48,44,47,49,46,42,44,40,45,42,
                                     44,40,42,38,40,36,40,37,40,35,37,34,35,31,48,
                                     47,50,46,48,44,47,43,47,43,46,42,44,40,42,38,
                                     42,39,42,38,40,36,38,34,38,34,37,32,34,30,32,27};
__interrupt void USCI_B0_ISR(void);
__interrupt void TIMERB0_ISR(void);
__interrupt void PORT1_ISR(void);

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;   // Stop watchdog timer
  
  initMC();
  initPWM(pwm_anzahl);
  initR2R();
  initSPI();
  initUART();
  initTIMER();
  __bis_SR_register(GIE);                   // Aktiviert allgemeine Interrupts
  
  while(1)
  {
    // warten auf Interrupts
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// SPI Interrupt Service Routine
#pragma vector = USCI_B0_VECTOR
__interrupt void USCI_B0_ISR(void)
{
  if (msb_or_lsb == 0)
  {
    rx_data = UCB0RXBUF;          // Auslesen 1te byte
    while(UCB0IFG == UCRXIFG);    // Warten bis fertig ausgelesen
    adc_value = rx_data << 8;     // Schreiben der msb's in adc_value
    msb_or_lsb = 1;
    
    UCB0TXBUF = 0x00;             // Senden 2tes leeres byte
  }
  else
  {
    rx_data = UCB0RXBUF;    // Auslesen 2tes byte
    while(UCB0IFG == UCRXIFG);  // Warten bis fertig ausgelesen
    P2OUT |= BIT3;              // CS High -> 16 Bit übertragung fertig
    
    adc_value |= rx_data;       // Schreiben der lsb's in adc_value
    msb_or_lsb = 0;
    
    if (vorlauf > 3)                                          
    {
      sum += adc_value;
      ++mittelwert_counter;
    }
    
    if (mittelwert_counter > mittelwert_anzahl)                                  
    {
      // uart_counter zum zuordnen der Werte senden:
      tx_data = uart_counter >> 8;                                              // MSBytes senden 
      UCA1TXBUF = tx_data;
      while(!(UCTXIFG == UCA1IFG));
      
      tx_data = uart_counter & 0x00FF;                                          // LSBytes senden
      UCA1TXBUF = tx_data;
      while(!(UCTXIFG == UCA1IFG));
      
      ++uart_counter;                                                           
        
      // Mittelwert ueber uart senden:
      mittelwert = sum >> mittelwert_aufloesung;                                // Summe der Werte durch 2^mittelwert_aufloesung teilen
      
      if (r2r_korrektur == 1)
      {
        mittelwert = mittelwert + korrektur_array[r2r_counter];                 // korrektur des systematischen Fehlers des r2r-Netzwerkes
      }
      
      tx_data = mittelwert >> 8;                                                // MSBytes senden 
      UCA1TXBUF = tx_data;
      while(!(UCTXIFG == UCA1IFG));
      
      tx_data = mittelwert & 0x00FF;                                            // LSBytes senden
      UCA1TXBUF = tx_data;
      while(!(UCTXIFG == UCA1IFG));
      
      // Mittelwert counter ruecksetzen und eventuell pwm/r2r erhöhen
      mittelwert_counter = 0;
      sum = 0;
      
      ++pwm_on;
      TA1CCR1 = pwm_on;
      if (pwm_on > pwm_anzahl)
      {
        pwm_on = 0;
        TA1CCR1 = pwm_on;                                                       // PWM output erhöhen
        
        ++r2r_counter;
        P6OUT = r2r_counter;                                                    // R2R Output erhöhen
      }
    }
  }
}

//TimerB0 Interrupt Service Routine 
#pragma vector = TIMERB0_VECTOR
__interrupt void TIMERB0_ISR(void) 
{
  if(r2r_counter > 0xFF)                                                        // gruenes LED signalisiert, dass das Programm fertig ist.
  {
    P4OUT ^= BIT7;
  } else
  {
    P1OUT ^= BIT0;
  }
  
  P2OUT &= ~BIT3;
  __delay_cycles(10);
  UCB0TXBUF = 0x00;
  
  if (vorlauf < 6)
  {
    ++vorlauf;
  }
  TB0CCTL0 &= ~CCIFG;                                                           // Interruptflag zurücksetzen
}

// PORT Interrupt Service Routine
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  __delay_cycles(30000);                                                        // Prellen des Tasters abwarten
  
  P1IFG &= ~BIT1;                                                               // Interruptflag löschen
  TB0CCTL0 |= CCIE;                                                             // Interrupt enable von TimerB0 => Programm startet
}
