#include "msp430F5529.h"
#include "lib/init.h"

//Einstellungen
#define pwm_aufloesung 7                                                        
#define mittelwert_aufloesung 0                                               

// Regelungsmodus
int regelung = 1;
int sollwert = 9930;            // Digitalwert, welcher ausgegeben soll
int x = 0;                   // Digitalwert, welcher ausgegeben wird
int e;                          
int sum_i = 0;

// Globale Variablen Deklaration
unsigned int uart_counter = 0;
unsigned int r2r_counter = 0;

unsigned int pwm_on = 0;                                                        // Pulsbreitenverh�ltniss
unsigned int pwm_anzahl = (1 << pwm_aufloesung) - 1;

unsigned int mittelwert = 0;
unsigned int mittelwert_counter = 0;
unsigned int mittelwert_anzahl = (1 << mittelwert_aufloesung)- 1;
long sum = 0;     

char rx_data;
char tx_data;

unsigned int adc_value;
unsigned int msb_or_lsb; 

unsigned int vorlauf = 0;                                                           // ersten beiden Werte des ADC's sind zu "l�schen"

unsigned int korrektur_bool = 1;
unsigned int dig_out;
int korrektur_array_12Bit[256] = { 0,-3,-3,-3,-3,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-1,
                        -2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0, 0, 0, 0, 0,
                        -2,-2,-2,-2,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,
                        -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
                        -1,-1,-1,-1,-1,-1,-1, 0,-1, 0, 0, 0, 0, 0, 0, 0, 
                         0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2,
                         0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1,
                         1, 1, 1, 1, 1, 1, 1, 2, 1, 2, 2, 2, 2, 2, 2, 2,
                        -2,-2,-3,-2,-2,-2,-2,-2,-2,-2,-2,-1,-2,-1,-1,-1,
                        -2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1, 0,
                        -2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-2,-1,-2,-1,
                        -2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1, 0,-1, 0,
                        -2,-2,-3,-2,-2,-2,-2,-1,-2,-2,-2,-1,-2,-1,-1,-1,
                        -1,-1,-1,-1,-1,-1,-1, 0,-1, 0,-1, 0, 0, 0, 0, 0,
                        -2,-1,-2,-1,-2,-1,-1,-1,-1,-1,-1,-1,-1, 0,-1, 0,
                        -1, 0,-1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 1, 1, 1, 2,}
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
  
  if (regelung == 1)
  {
    TA1CCR1 = sollwert & 0x007F;
    P6OUT =  sollwert >> 7;
  }
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
  if (msb_or_lsb == 1)
  {
    rx_data = UCB0RXBUF;    // Auslesen 2tes byte
    while(UCB0IFG == UCRXIFG);  // Warten bis fertig ausgelesen
    
    
    adc_value |= rx_data;       // Schreiben der lsb's in adc_value
    msb_or_lsb = 0;
  }

  if ((regelung == 1)  & (vorlauf > 3)) 
  {
    adc_value = adc_value & 0x7FFF;
    tx_data = adc_value >> 8;                                                // MSBytes senden 
    UCA1TXBUF = tx_data;
    while(!(UCTXIFG == UCA1IFG));
    
    tx_data = adc_value & 0x00FF;                                            // LSBytes senden
    UCA1TXBUF = tx_data;
    while(!(UCTXIFG == UCA1IFG));
    
    e = sollwert - adc_value;         // Abweichung MEssert von Sollwert
    sum_i = sum_i + e;                // Sumierung fuer den I-Anteil
    
    x = x + (e >> 2) + (sum_i >> 6); //  kp = 1/4 , A = 1/16
    
    // 16Bit x Wert auf das Stellglied geben
    TA1CCR1 = x & 0x007F;
    P6OUT =  x >> 7;
  } 
  else
  {
    break;
  }
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
    
    // Mittelwert counter ruecksetzen und eventuell pwm/r2r erh�hen
    mittelwert_counter = 0;
    sum = 0;
    
    ++pwm_on;
    if (pwm_on > pwm_anzahl)
    {
      pwm_on = 0;                                                       
      ++r2r_counter;                                                             
    }

    if (korrektur == 1)
    {
      dig_out = (r2r_counter << 4) | pwm_on;                      // Zusammenführen PWM und R2R
      dig_out -= korrektur_array_12Bit(r2r_counter);

      P6OUT = dig_out >> 4;                                       // R2R Output ausgeben
      TA1CCR1 = dig_out & 0x000F;                                 // PWM output ausgeben
    }
    else 
    {
      P6OUT = r2r_counter;                                        // R2R Output ausgeben
      TA1CCR1 = pwm_on;                                           // PWM output ausgeben
    }

  }
  P2OUT |= BIT3;              // CS High -> ADC soll wieder wandeln
  
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
  TB0CCTL0 &= ~CCIFG;                                                           // Interruptflag zur�cksetzen
}

// PORT Interrupt Service Routine
#pragma vector = PORT1_VECTOR
__interrupt void PORT1_ISR(void)
{
  __delay_cycles(30000);                                                        // Prellen des Tasters abwarten
  
  P1IFG &= ~BIT1;                                                               // Interruptflag l�schen
  TB0CCTL0 |= CCIE;                                                             // Interrupt enable von TimerB0 => Programm startet
}
