// INTERFACE -> SPI und UART-funktionen werden hier definiert

#include "msp430F5529.h"

int SPI_recieve(void)
{
  int msb_byte = 0;
  int lsb_byte = 0;
  
  UCB0TXBUF = 0;                         // 8 Flanken an Slave senden
  while((UCRXIFG & UCB0IFG) == 0);       // Auf Antwort warten
  msb_byte = UCB0RXBUF;                  // Antwort in msb_byte speichern
  
  UCB0TXBUF = 0;                         // 8 Flanken an Slave senden
  while((UCRXIFG & UCB0IFG) == 0);       // Auf Antwort warten
  lsb_byte = UCB0RXBUF;                  // Antwort in msb_byte speichern
  
  return (msb_byte << 8 + lsb_byte);     // bytes in 16bit int zusammensetzen
}

void UART_send(int two_bytes)
{
  char lsb_byte = two_bytes & 0x0F;
  char msb_byte = two_bytes >> 8;
  
  UCA1TXBUF = msb_byte;
  while((UCTXIFG & UCA1IFG) == 0);
  UCA1TXBUF = lsb_byte;
  while((UCTXIFG & UCA1IFG) == 0);
  
}