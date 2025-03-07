// INIT -> Alle Initfunktionen werden hier definiert
#include "msp430F5529.h"

void initMC(void)
{
  // Stop watchdog timer to prevent time out reset
  WDTCTL = WDTPW + WDTHOLD;
  
  //LEDs als Ausgänge
  P4DIR |= BIT7;        // Ausgang
  P4SEL &= ~BIT7;       // GIO PIN
  P4OUT &= ~BIT7;       // LED aus
  
  P1DIR |= BIT0;        // Ausgang
  P1SEL &= ~BIT0;       // GIO PIN
  P1OUT &= ~BIT0;       // LED aus
  
  // Button S2 als Start
  P1DIR &= ~BIT1;       // Eingang nach Tabelle 12-1 aus dem Userguide
  P1REN |= BIT1;        // PullUp/Pulldown aktiviert
  P1OUT |= BIT1;        // PullUp ausgewählt
  P1SEL &= ~BIT1;       // GIO PIN
  P1IES |= BIT1;        // Interrupt auf negative Flanke 
  P1IE = BIT1;          // Interrupt enable
  P1IFG &= ~BIT1;       // Interruptflag löschen
  
  // Port 2.6 als Eingang, um die Wandlung zu ueberwachen
  
  P2DIR &= ~BIT6;       // Eingang nach Tabelle 12-1 aus dem Userguide
  P2REN |= BIT6;        // PullUp/Pulldown aktiviert
  P2OUT &= ~BIT6;       // Pulldown ausgewählt
  P2SEL &= ~BIT6;       // GIO PIN
  P2IES &= ~BIT6;       // Interrupt auf positive Flanke 
  P2IE &= ~BIT6;        // Interrupt noch nicht erlauben
  P2IFG &= ~BIT6;       // Interruptflag löschen
}

void initPWM(int pwm_anzahl)
{
  // P2.0 ist Output der PWM
  P2DIR |= BIT0;        // P2.0 ist Ausgang
  P2SEL |= BIT0;        // P2.0 ist kein GPIO	
  
  // Timer A1 realisiert PWM
  TA1CCR0 = pwm_anzahl;                                                         // 
  TA1CCTL1 = OUTMOD_7;                                                          // CCR1 im Reset/Set Modus
  TA1CCR1 = 0;                                                                   
  TA1CTL = TASSEL_2 + MC_1 + TACLR;                                             // SMCLK, up mode, loesche TAR 	
}

void initTIMER(void)
{
  TB0CCR0 = 2873;                 // Timer alle 23 ms 2873
  TB0CTL = TBSSEL_2 + MC_1 + ID_3; // SMCLK/8, upmode, loesche TBR
  
  TB0CCTL0 &= ~CCIE;                                                            // Interrupt noch nicht frei gegeben
}

void initR2R(void)
{
  P6DIR = 0xFF; // Jeder Pin ist Ausgang
  P6SEL = 0x00; // GIO-PIN
  P6OUT = 0x00; // Jeder Pin erstmals auf 0
}

// INIT von SPI/UART nach fogendem Schema (Siehe Userguide S894):
// 1)SWRST-Mode
// 2)Config
// 3)Port Config
// 4)End SWRST-Mode
// 5)Enable Interrupts

void initUART(void)
{
  UCA1CTL1 |= UCSWRST;          // SW-Reset Mode
  
  UCA1CTL0 = 0x00;              // Betriebsart des UART: Asynchron, 8 Datenbits, 1 Stopbit, kein Paritybit, LSB zuerst senden/empfangen 
  UCA1CTL1 |= UCSSEL__SMCLK;    // SMCLK als Taktquelle ausw?hlen
  UCA1BR0 = 109;                // Lowbyte, Baudrate einstellen
  UCA1BR1 = 0;                  // Highbyte, Baudrate einstellen
  UCA1MCTL |= UCBRS1;           // Modulator einstellen
  UCA1STAT = 0x00;              // alle moeglichen Flags loeschen
  UCA1ABCTL = 0x00;             // keine Auto-Baudrate-Detektion
  
  P4OUT |= BIT4;                // Idle HIGH
  P4SEL |= BIT4;                // RX, kein GIO
  P4SEL |= BIT5;                // TX, kein GIO
  
  UCA1CTL1 &= ~UCSWRST;         // Aus SW-Reset in Betrieb nehmen nach Konfiguration
  
  UCA1IE |= UCRXIE;             // Interrupt enable
}

void initSPI(void)
{
  UCB0CTL1 |= UCSWRST;                                          // SW-Reset Mode
  
  UCB0CTL0 &= ~UCCKPH;                                          // Daten werden zur ersten Flanke geändert
  UCB0CTL0 |= UCCKPL;                                           // Clock Polarität -> Inaktiv = high
  UCB0CTL0 |= UCMSB;                                            // MSB zuerst
  UCB0CTL0 &= ~UC7BIT;                                          // 8-Bit Data
  UCB0CTL0 |= UCMST;                                            // Mastermode
  //UCB0CTL0 |= 0x00;                                           // funktioniert nicht mit UCMODE_2(CS wird nicht automatisch gesetzt?) 
  UCB0CTL0 |= UCSYNC;                                           // Synchron -> SPI
  
  UCB0CTL1 |= UCSSEL__SMCLK;                                    // SMCLK als Taktquelle auswaehlen
  
  
  UCB0BR0 = 10;                                               // SMCLK wird durch 10 geteilt auf 100khz
  UCB0BR1 = 0;
  
  //Pins für SPI
  P3SEL |= BIT0;        // P3.0 ist MOSI (nicht verbunden)
  P3SEL |= BIT1;        // P3.1 ist MISO (Serial Data Output des ADC)
  P3SEL |= BIT2;        // P3.2 ist SPI CLK
  
  //P2SEL |= BIT3;      // P2.3 ist Chip Select
  P2DIR |= BIT3;        // Ausgang
  P2SEL &= ~BIT3;       // GIO PIN
  P2OUT |= BIT3;        // CS HIGH
  
  UCB0CTL1 &= ~UCSWRST;                                 // Aus SW-Reset in Betrieb nehmen nach Konfiguration
  
  UCB0IE |= UCRXIE;                                     // Interrupt enable
  //UCB0IE |= UCTXIE;
  UCB0IFG &= ~UCRXIE;                                   // Clear Flags
  UCB0IFG &= ~UCTXIE;
}