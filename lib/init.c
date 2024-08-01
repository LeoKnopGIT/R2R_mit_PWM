// INIT -> Alle Initfunktionen werden hier definiert
#include "msp430F5529.h"

void initMC(void)
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  __bis_SR_register(GIE);     // Global Interrupt Enable
  
  //LEDs als Ausgänge
  P4DIR |= BIT7;        // Ausgang
  P4SEL &= ~BIT7;       // GIO PIN
  P4OUT &= ~BIT7;       // LED aus
  
  P1DIR |= BIT0;        // Ausgang
  P1SEL &= ~BIT0;       // GIO PIN
  P1OUT &= ~BIT0;       // LED aus
  
  // Button S2 als Start
  P1IFG &= ~BIT1;       // Interruptflag löschen
  
  P1DIR &= ~BIT1;       // Eingang nach Tabelle 12-1 aus dem Userguide
  P1REN |= BIT1;        // PullUp/Pulldown aktiviert
  P1OUT |= BIT1;        // PullUp ausgewählt
  
  P1SEL &= ~BIT1;       // GIO PIN
  //P1IES |= BIT1;        // Interrupt auf negative Flanke 
  P1IE = 0x00;          // Interrupt blockieren
}

void initPWM(int pwm_anzahl, int pwm_on)
{
  // P2.0 ist Output der PWM
  P2DIR |= BIT0;        // P2.0 ist Ausgang
  P2SEL |= BIT0;        // P2.0 ist kein GPIO	
  
  // Timer A0 realisiert PWM
  TA1CCR0 = pwm_anzahl - 1;                             // "pwm_aufloesung" -Bit Auflusung für PWM   f_PWM=1,048MHz/8/128=1,04kHz
  TA1CCTL1 = OUTMOD_7;                                  // CCR1 im Reset/Set Modus
  TA1CCR1 = pwm_on;                                     // PWM_on bestimmt Pulsbreite 
  TA1CTL = TASSEL_2 + MC_1 + TACLR + ID_2 + ID_1;       // SMCLK, up mode, loesche TAR f_clk/8	  
}

void initR2R(void)
{
  P6DIR = 0xFF; // Jeder Pin ist Ausgang
  P6OUT = 0x00; // Jeder Pin erstmals auf 0
}

// INIT von SPI/UART nach fogendem Schema (Siehe Userguide S894):
// 1)SWRST-Mode
// 2)Config
// 3)Port Config
// 4)End SWRST-Mode
// 5)Enable Interrupts

void initSPI(void)
{
  UCB0CTL1 |= UCSWRST;                                          // SW-Reset Mode
  UCB0CTL0 |= UCCKPL + UCMSB + UCMST + UCSYNC;                  // SPI-Konfiguration (siehe Userguide S939)
  UCB0CTL1 |= UCSSEL1;                                          // SMCLK als Taktquelle auswaehlen
  
  //Pins für SPI
  P3SEL |= BIT0;        // P3.0 ist MISO (Serial Data Output des ADC)
  
  P2SEL |= BIT3;        // P2.3 ist Chip Select
  
  UCB0CTL1 &= ~UCSWRST;                                 // Aus SW-Reset in Betrieb nehmen nach Konfiguration
  // UCB0IE |= UCRXIE + UCTXIE;                            // Interrupt enable Problem -> Drekt interrupt ohne routione
}

void initUART(void)
{
  UCA1CTL1 |= UCSWRST;          // SW-Reset Mode
  UCA1CTL0 = 0x00;              // Betriebsart des UART: Asynchron, 8 Datenbits, 1 Stopbit, kein Paritybit, LSB zuerst senden/empfangen 
  UCA1CTL1 |= UCSSEL1;          // SMCLK als Taktquelle ausw?hlen
  UCA1BR0 = 109;                // Lowbyte, Baudrate einstellen
  UCA1BR1 = 0;                  // Highbyte, Baudrate einstellen
  UCA1MCTL |= UCBRS1;           // Modulator einstellen
  UCA1STAT = 0x00;              // alle moeglichen Flags loeschen
  UCA1ABCTL = 0x00;             // keine Auto-Baudrate-Detektion
  UCA1CTL1 &= ~UCSWRST;         // Aus SW-Reset in Betrieb nehmen nach Konfiguration
  UCA1IE |= UCRXIE;             // Interrupt enable
}