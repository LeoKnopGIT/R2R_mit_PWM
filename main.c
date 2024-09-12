#include "msp430F5529.h"
#include "lib/init.h"

#define pwm_aufloesung 4                                                        
#define mittelwert_aufloesung 0                                                 

// Globale Variablen Deklaration
unsigned int uart_counter = 0;
unsigned int r2r_counter = 0;

unsigned int pwm_on = 0;                                                        // Pulsbreitenverhältniss
unsigned int pwm_anzahl = (1 << pwm_aufloesung) - 1;

unsigned int mittelwert = 0;
unsigned int mittelwert_counter = 0;
unsigned int mittelwert_anzahl = (1 << mittelwert_aufloesung)- 1;
long sum = 0;     

char rx_data;
char tx_data;

unsigned int adc_value;
unsigned int msb_or_lsb;

unsigned int vorlauf = 0;                                                           // ersten beiden Werte des ADC's sind zu "löschen"

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
  
  ++vorlauf;
  
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
