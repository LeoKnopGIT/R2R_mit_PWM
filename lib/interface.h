// INTERFACE -> SPI und UART-Funktionen werden hier deklkariert
#ifndef INTERFACE_H
#define INTERFACE_H

int SPI_recieve(void);
void UART_send(int two_bytes);

#endif