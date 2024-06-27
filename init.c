// INIT -> Alle Initfunktionen werden hier definiert

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
  P1DIR &= ~BIT1;       // Eingang
  P1SEL &= ~BIT1;       // GIO PIN
  P1IES |= BIT1;        // Interrupt auf negative Flanke (PullUp aktiviert)
  P1IE |= BIT1;         // Interrupt freigeben
}

void initPWM(void)
{
  // P2.0 ist Output der PWM
  P2DIR |= BIT0;        // P2.0 ist Ausgang
  P2SEL |= BIT0;        // P2.0 ist kein GPIO	
  
  // Timer A0 realisiert PWM
  TA1CCR0 = pwm_anzahl - 1;                           // "pwm_aufloesung" -Bit Auflusung für PWM   f_PWM=1,048MHz/8/128=1,04kHz
  TA1CCTL1 = OUTMOD_7;                                  // CCR1 im Reset/Set Modus
  TA1CCR1 = pwm_on;                                     // PWM_on bestimmt Pulsbreite 
  TA1CTL = TASSEL_2 + MC_1 + TACLR + ID_2 + ID_1;       // SMCLK, up mode, loesche TAR f_clk/8	  
}

void initR2R(void)
{
  P6DIR = 0xFF; // Jeder Pin ist Ausgang
  P6OUT = 0x00; // Jeder Pin erstmals auf 0
}
void initSPI(void)
{
}

void initUART(void)
{
  UCA1CTL0 = 0x00;              // Betriebsart des UART: Asynchron, 8 Datenbits, 1 Stopbit, kein Paritybit, LSB zuerst senden/empfangen 
  UCA1CTL1 |= UCSSEL1;          // SMCLK als Taktquelle ausw?hlen
  UCA1BR0 = 109;                // Lowbyte, Baudrate einstellen
  UCA1BR1 = 0;                  // Highbyte, Baudrate einstellen
  UCA1MCTL |= UCBRS1;           // Modulator einstellen
  UCA1STAT = 0x00;              // alle moeglichen Flags loeschen
  UCA1ABCTL = 0x00;             // keine Auto-Baudrate-Detektion
  UCA1CTL1 &= ~UCSWRST;         // Die UART in den normalen Betrieb versetzen nachdem zuvor alles Konfiguriert wurde, siehe UserGuide Seite 894
  UCA1IE |= UCRXIE;             //Interrupt fuer die UART aktivieren: wenn ein Byte empfangen wurde (RxD-Interrupt), diese Zeile MUSS zwingend nach dem Reset des UART Moduls erfolgen
}