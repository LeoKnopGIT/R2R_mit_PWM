#ifndef INIT_H
#define INIT_H
#include "msp430F5529.h"

// INIT -> Alle Initfunktionen werden hier deklariert
void initMC(void);
void initPWM(void);
void initR2R(void);
void initSPI(void);
void initUART(void);

#endif